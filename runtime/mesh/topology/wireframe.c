#include "wireframe.h"
#include "../backend/buffer.h"
#include "../backend/shadow_pass.h"
#include "../runtime/geometry/edge/edge.h"
#include "../runtime/geometry/line/line.h"
#include "../utils/math.h"
#include "core.h"
#include "webgpu/webgpu.h"

// anchor
static int mesh_topology_wireframe_anchor_create(MeshTopologyWireframeAnchor *,
                                                 size_t, vindex_t);
static int mesh_topology_wireframe_anchor_expand(MeshTopologyWireframeAnchor *);

static bool mesh_topology_wireframe_is_face(VertexIndex *);

static void mesh_topology_wireframe_store_unique_edges(EdgeHashSet *,
                                                       MeshTopology *);
static void mesh_topology_wireframe_create_points(EdgeHashSet *, MeshTopology *,
                                                  MeshTopologyWireframe *);

// anchor list
static int
mesh_topology_wireframe_anchor_list_expand(MeshTopologyWireframeAnchorList *);
static int
mesh_topology_wireframe_anchor_list_create(MeshTopologyWireframeAnchorList *,
                                           size_t);

/**
 Go through unique edges set add populate temp vertex & index array
 */
void mesh_topology_wireframe_create_points(EdgeHashSet *edges,
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
  }
}

void mesh_topology_wireframe_store_unique_edges(EdgeHashSet *edges,
                                                MeshTopology *src_topo) {

  // store edges
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
   However this method is not robust shall be replaced with a more explicit way.
   (TODO)
 */
bool mesh_topology_wireframe_is_face(VertexIndex *index) {

  vindex_t *id = index->entries;

  for (size_t i = 1; i < MIN(6, index->length); i++) {
    if (id[i] != id[i - 1] + 1)
      return true;
  }

  return false;
}

int mesh_topology_wireframe_anchor_list_expand(
    MeshTopologyWireframeAnchorList *list) {

  size_t new_capacity = 2 * list->capacity;
  MeshTopologyWireframeAnchor *temp = (MeshTopologyWireframeAnchor *)realloc(
      list->entries, sizeof(MeshTopologyWireframeAnchor) * new_capacity);

  if (temp == NULL) {
    perror("Couldn't expand line mesh anchor list\n");
    return MESH_TOPOLOGY_WIREFRAME_ALLOC_FAIL;
  }

  list->entries = temp;
  list->capacity = new_capacity;

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

int mesh_topology_wireframe_anchor_list_create(
    MeshTopologyWireframeAnchorList *list, size_t capacity) {

  list->capacity = capacity;
  list->length = 0;
  list->entries = malloc(sizeof(MeshTopologyWireframeAnchor) * capacity);

  if (list->entries == NULL) {
    perror("Couldn't create new line mesh anchor list\n");
    list->capacity = 0;
    return MESH_TOPOLOGY_WIREFRAME_ALLOC_FAIL;
  }

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

int mesh_topology_wireframe_anchor_expand(MeshTopologyWireframeAnchor *anchor) {

  size_t new_capacity = 2 * anchor->capacity;
  vindex_t *temp =
      (vindex_t *)realloc(anchor->entries, sizeof(vindex_t) * new_capacity);

  if (temp == NULL) {
    perror("Couldn't expand line mesh anchor list\n");
    return MESH_TOPOLOGY_WIREFRAME_ALLOC_FAIL;
  }

  anchor->entries = temp;
  anchor->capacity = new_capacity;

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

int mesh_topology_wireframe_anchor_create(MeshTopologyWireframeAnchor *anchor,
                                          size_t capacity, vindex_t index) {

  anchor->capacity = capacity;
  anchor->length = 0;
  anchor->anchor = index;
  anchor->entries = malloc(sizeof(vindex_t) * capacity);

  if (anchor->entries == NULL) {
    perror("Couldn't create new line mesh anchor\n");
    anchor->capacity = 0;
    return MESH_TOPOLOGY_WIREFRAME_ALLOC_FAIL;
  }

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

int mesh_topology_wireframe_create(MeshTopology *src_topo,
                                   MeshTopologyWireframe *dest_topo,
                                   const WGPUDevice *device,
                                   const WGPUQueue *queue) {

  /*
    Create a edge hash set to store unique edges
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

   */

  /* Check if source topology is a 'line' of 'face' type structure
     Line type have an even index length ( len(i) % 2 == 0)
     Whereas Face type have an odd index length as they are composed of
     triangles.
     If the source topology is a Line type, then we need to triangulate first
     before.
   */

  EdgeHashSet edges;
  edge_hash_set_create(&edges, 40);

  // store unique edges
  mesh_topology_wireframe_store_unique_edges(&edges, src_topo);

  // arrays from edges
  size_t vertex_capacity = edges.length * LINE_VERTEX_COUNT * VERTEX_STRIDE;
  vattr_t wireframe_vertex_attribute[vertex_capacity];
  dest_topo->attribute = (VertexAttribute){
      .entries = wireframe_vertex_attribute,
      .capacity = vertex_capacity,
      .length = 0,
      .buffer = NULL,
  };

  size_t index_capacity = edges.length * LINE_INDEX_COUNT;
  vindex_t wireframe_index_attribute[index_capacity];
  dest_topo->index = (VertexIndex){
      .entries = wireframe_index_attribute,
      .capacity = index_capacity,
      .length = 0,
      .buffer = NULL,
  };

  // create points from unique edges
  mesh_topology_wireframe_create_points(&edges, src_topo, dest_topo);

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

  // create anchor list
  // mesh_topology_wireframe_anchor_list_create(
  //    src_topo->anchors,
  //    MESH_TOPOLOGY_WIREFRAME_ANCHOR_LIST_DEFAULT_CAPACITY);

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor(MeshTopologyWireframe *wireframe,
                               const vindex_t anchor_index) {

  return NULL;
}

VertexAttribute *
mesh_topology_wireframe_anchor_attribute(MeshTopologyWireframe *mesh,
                                         const vindex_t anchor_index) {

  return NULL;
}

void mesh_topology_wireframe_anchor_set_attribute(MeshTopologyWireframe *mesh,
                                                  const vindex_t anchor_index,
                                                  const VertexAttribute *va) {}

int mesh_topology_wireframe_anchor_insert(MeshTopologyWireframeAnchor *anchor,
                                          vindex_t index) {

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

int mesh_topology_wireframe_anchor_list_insert(
    MeshTopologyWireframeAnchorList *list,
    MeshTopologyWireframeAnchor *anchor) {

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor_list_new(MeshTopologyWireframeAnchorList *list) {
  return NULL;
}

MeshTopology mesh_topology_wireframe_vertex(MeshTopologyWireframe *topo) {
  return (MeshTopology){
      .attribute = &topo->attribute,
      .index = &topo->index,
  };
}
