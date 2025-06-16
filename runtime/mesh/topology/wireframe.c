#include "wireframe.h"
#include "../backend/buffer.h"
#include "../backend/shadow_pass.h"
#include "../runtime/geometry/edge/edge.h"
#include "../runtime/geometry/line/line.h"
#include "../utils/math.h"
#include "../utils/system.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static bool mesh_topology_wireframe_is_face(VertexIndex *);

static void mesh_topology_wireframe_store_unique_edges(EdgeHashSet *,
                                                       MeshTopology *);
static void mesh_topology_wireframe_create_points(EdgeHashSet *,
                                                  MeshTopologyAnchorList *,
                                                  MeshTopology *,
                                                  MeshTopologyWireframe *);

/**
   Create lines and anchors from base topology

    ================ 1. EDGE CREATION PHASE ================

   Create a edge hash set to store unique edges.
   To store unique data, hash tables are more efficient since we can directly
   check if the hash exist via the hashing function instead of every time
   traverse the array and compate the data.

      Object                                  Hash Table
    .--------.                              .-------------.
    | attr a | --.   .-------------.        |     ..      |
    |--------|   '-- |   Hashing   | --.    |-------------|
    | attr b | --'   '-------------'   |    |     ..      |
    '--------'                         |    |-------------|
                                       '--> |     34      |
                                            |-------------|
                                            |     ..      |
                                            '-------------'

               Data Attributes + Hash = Index



    =================== 2. CLUSTER PHASE ==================

    Create anchors hash set with base position as key:
    If positions a, b, c are equals:

        Positions                     Wireframe Index (Cluster)

                                           .-----------.
     [0] Xa Ya Za ... ----------.          | Hash(abc) |
     [1] Xb Yb Zb ... -------------------> |---.---.---|
     [2] Xc Yc Zc ... ----------'          | 0 | 1 | 2 |
                                           '---'---'---'



     ================== 3. MAPPING PHASE ==================

     Now that base indexes are clustered based on their positions
     we create a new anchor list where we map each base index to a wireframe
     cluster based on the base position.

     .-----------------------.---------------------.---------------------.
     |         Step          |        Input        |       Output        |
     |-----------------------+---------------------+---------------------|
     |     .------------.    |                     |                     |
     |     | Base Index |    |  0                  |                     |
     |     '------------'    |                     |                     |
     |           v           |---------------------+---------------------|
     |   .---------------.   |                     |                     |
     |   | Base Position |   |  0                  |  [1.0, -1.0, 1.0]   |
     |   '---------------'   |                     |                     |
     |           v           |---------------------+---------------------|
     |      .----------.     |                     |                     |
     |      |   Hash   |     |  [1.0, -1.0, 1.0]   |  348                |
     |      '----------'     |                     |                     |
     |           v           |---------------------|---------------------|
     |     .------------.    |                     |                     |
     |     |   Cluster  |    |  348                |  [2,3,4,1]          |
     |     '------------'    |                     |                     |
     |           v           |---------------------|---------------------|
     |     .-------------.   |                     |                     |
     |     |     Map     |   |  0                  |  [2,3,4,1]          |
     |     '-------------'   |                     |                     |
     '-----------------------'---------------------'---------------------'

     The point of remapping the clusters (initially ordered by hash) is to have
     an easier access to them afterward when we need to update the wireframe
     vertex based on the base topology.

     The mapped version will help to directly target the cluster based on the
     base index.

     This extra step is due to the fact that out cluster list is created based
   on unique edges, which doesn't include all the base index topology.


 */
int mesh_topology_wireframe_create(MeshTopology *src_topo,
                                   MeshTopologyWireframe *dest_topo,
                                   const WGPUDevice *device,
                                   const WGPUQueue *queue) {
  EdgeHashSet edges;
  edge_hash_set_create(&edges, 40);

  // store unique edges
  mesh_topology_wireframe_store_unique_edges(&edges, src_topo);

  // arrays from edges
  size_t vertex_capacity = edges.length * LINE_VERTEX_COUNT * VERTEX_STRIDE;
  dest_topo->attribute = (VertexAttribute){
      .entries = malloc(vertex_capacity * sizeof(vattr_t)),
      .capacity = vertex_capacity,
      .length = 0,
      .buffer = NULL,
  };

  size_t index_capacity = edges.length * LINE_INDEX_COUNT;
  dest_topo->index = (VertexIndex){
      .entries = malloc(index_capacity * sizeof(vindex_t)),
      .capacity = index_capacity,
      .length = 0,
      .buffer = NULL,
  };

  MeshTopologyAnchorList hashed_anchors;
  // create cluster anchor list
  mesh_topology_anchor_list_create(&hashed_anchors,
                                   MESH_TOPOLOGY_ANCHOR_LIST_DEFAULT_CAPACITY);

  // create mapped anchor list
  mesh_topology_anchor_list_create(&dest_topo->anchors,
                                   src_topo->index->length);

  // create points from unique edges
  mesh_topology_wireframe_create_points(&edges, &hashed_anchors, src_topo,
                                        dest_topo);

  // map wireframe index cluster based on base topology index
  mesh_topology_anchor_list_map(&hashed_anchors, src_topo, &dest_topo->anchors);

  // upload vertex attributes
  buffer_create(&dest_topo->attribute.buffer,
                &(CreateBufferDescriptor){
                    .queue = queue,
                    .device = device,
                    .data = (void *)dest_topo->attribute.entries,
                    .size = dest_topo->attribute.length * sizeof(vattr_t),
                    .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });

  // upload vertex index
  buffer_create(&dest_topo->index.buffer,
                &(CreateBufferDescriptor){
                    .queue = queue,
                    .device = device,
                    .data = (void *)dest_topo->index.entries,
                    .size = dest_topo->index.length * sizeof(vindex_t),
                    .usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

/**
   Convert MeshTopology Wireframe to a simple Mesh Topology
 */
MeshTopology mesh_topology_wireframe_vertex(MeshTopologyWireframe *topo) {
  return (MeshTopology){
      .attribute = &topo->attribute,
      .index = &topo->index,
  };
}

/**
   Travert the base topology index, retrieve the position and update relative
   wireframe vertex attribute based on achor's index.
 */
int mesh_topology_wireframe_update(const MeshTopologyBase *base_topo,
                                   MeshTopologyWireframe *dest_topo,
                                   const WGPUQueue *queue) {

  for (size_t b = 0; b < base_topo->index.length; b++) {

    vindex_t base_index = base_topo->index.entries[b];
    vattr_t *base_vertex =
        &base_topo->attribute.entries[base_index * VERTEX_STRIDE];

    // look up base index in anchor list
    MeshTopologyAnchor *anchor = &dest_topo->anchors.entries[base_index];

    //  adjust anchor's linked index attributes
    if (anchor != NULL) {
      for (size_t w = 0; w < anchor->length; w++) {
        vindex_t wireframe_index = anchor->entries[w];
        vattr_t *wireframe_vertex =
            &dest_topo->attribute.entries[wireframe_index * VERTEX_STRIDE];

        // update wireframe vertex position
        memcpy(wireframe_vertex, base_vertex, sizeof(vertex_position));
      }

    } else {
      return MESH_TOPOLOGY_ANCHOR_UNSET;
    }
  }

  // update buffer or use map_write for direct link with CPU
  wgpuQueueWriteBuffer(*queue, dest_topo->attribute.buffer, 0,
                       dest_topo->attribute.entries,
                       dest_topo->attribute.length * sizeof(vattr_t));

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

/**
 Go through unique edges set add populate temp vertex & index array.
 Also add the newly creater index to the anchor array.
 */
void mesh_topology_wireframe_create_points(EdgeHashSet *edges,
                                           MeshTopologyAnchorList *hashed_list,
                                           MeshTopology *src_topo,
                                           MeshTopologyWireframe *dest_topo) {
  // TODO: make dynamic wireframe color
  vec3 color = {randf(), randf(), randf()};

  for (size_t l = 0; l < edges->length; l++) {

    size_t index = edges->occupied[l];
    EdgeBucket *current_edge = &edges->entries[index];

    // base vertex
    int base_index = current_edge->key[0];
    float *base_attributes =
        &src_topo->attribute->entries[base_index * VERTEX_STRIDE];
    Vertex base_vertex = vertex_from_array(base_attributes);

    // opposite vertex
    int opp_index = current_edge->key[1];
    float *opp_attributes =
        &src_topo->attribute->entries[opp_index * VERTEX_STRIDE];
    Vertex opp_vertex = vertex_from_array(opp_attributes);

    // add points to vertex attributes and index
    line_add_point(base_vertex.position, opp_vertex.position, color,
                   &dest_topo->attribute, &dest_topo->index);

    /**
       Based on the line add point function, the triangles are build with the
       following pattern:

              A
         c    .    b        Pattern:    a, b, c, a, c, d
          o---;---o                     ^  ^  ^        ^
          |\  ;   |         Offset:     0  1  2        5
          | \ ;   |                     ^  ^  ^        ^
          |  \;   |         Anchor:     A  B  B        A
          |   ;   |
          |   ;\  |
          |   ; \ |
          |   ;  \|
          o---;---o
         d    '    a
              B
     */

    // append base anchor
    size_t index_len = dest_topo->index.length;
    vindex_t temp_base_index[] = {
        dest_topo->index.entries[index_len - 6],
        dest_topo->index.entries[index_len - 1],
    };

    mesh_topology_anchor_list_insert(hashed_list, &base_vertex.position,
                                     temp_base_index, 2);

    // append opp anchor
    vindex_t temp_opp_index[] = {
        dest_topo->index.entries[index_len - 5],
        dest_topo->index.entries[index_len - 4],
    };

    mesh_topology_anchor_list_insert(hashed_list, &opp_vertex.position,
                                     temp_opp_index, 2);
  }
}

void mesh_topology_wireframe_store_unique_edges(EdgeHashSet *edges,
                                                MeshTopology *src_topo) {

  // store edges

  /* Check if source topology is a 'line' of 'face' type structure
     Line type have an even index length ( len(i) % 2 == 0)
     Whereas Face type have an odd index length as they are composed of
     triangles.
     If the source topology is a Line type, then we need to triangulate first
     before.
   */

  bool is_face = mesh_topology_wireframe_is_face(src_topo->index);
  int stride = is_face ? 3 : 2;
  for (int i = 0; i < src_topo->index->length; i += stride) {

    unsigned int a = src_topo->index->entries[i];
    unsigned int b = src_topo->index->entries[i + 1];

    EdgeKey ab = {MIN(a, b), MAX(a, b)};

    edge_hash_set_insert(edges, ab);

    if (is_face) {
      unsigned int c = src_topo->index->entries[i + 2];
      EdgeKey bc = {MIN(b, c), MAX(b, c)};
      EdgeKey ca = {MIN(a, c), MAX(a, c)};

      edge_hash_set_insert(edges, bc);
      edge_hash_set_insert(edges, ca);
    }
  }
}

/**
   Detect if a index list is Face or Line type.
   However this method is not robust shall be replaced with a more type or
   enum-based way. (TODO)
 */
bool mesh_topology_wireframe_is_face(VertexIndex *index) {

  vindex_t *id = index->entries;

  for (size_t i = 1; i < MIN(6, index->length); i++) {
    if (id[i] != id[i - 1] + 1)
      return true;
  }

  return false;
}
