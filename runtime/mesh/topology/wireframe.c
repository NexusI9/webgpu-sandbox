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
static void
mesh_topology_wireframe_create_points(EdgeHashSet *,
                                      MeshTopologyWireframeAnchorList *,
                                      MeshTopology *, MeshTopologyWireframe *);

// anchor
static int mesh_topology_wireframe_anchor_create(MeshTopologyWireframeAnchor *,
                                                 size_t);
static int mesh_topology_wireframe_anchor_expand(MeshTopologyWireframeAnchor *);

static MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor(MeshTopologyWireframe *,
                               const vertex_position *);

static int mesh_topology_wireframe_anchor_insert(MeshTopologyWireframeAnchor *,
                                                 vindex_t *, size_t);

static void mesh_topology_wireframe_anchor_print(MeshTopologyWireframeAnchor *);

static VertexAttribute *
mesh_topology_wireframe_anchor_attribute(MeshTopologyWireframe *,
                                         const vindex_t);

static void mesh_topology_wireframe_anchor_set_attribute(
    MeshTopologyWireframe *, const vindex_t, const VertexAttribute *);

// anchor list
static uint32_t mesh_topology_wireframe_anchor_list_hash(vertex_position,
                                                         size_t);
static int
mesh_topology_wireframe_anchor_list_expand(MeshTopologyWireframeAnchorList *);
static int
mesh_topology_wireframe_anchor_list_create(MeshTopologyWireframeAnchorList *,
                                           size_t);

static int mesh_topology_wireframe_anchor_list_insert(
    MeshTopologyWireframeAnchorList *, vertex_position *, vindex_t *, size_t);

static MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor_list_new(MeshTopologyWireframeAnchorList *,
                                        vertex_position *);

static MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor_list_find(MeshTopologyWireframeAnchorList *,
                                         vertex_position *);

uint32_t mesh_topology_wireframe_anchor_list_hash(vertex_position key,
                                                  size_t capacity) {

  MeshTopologyWireframeAnchorKey fx = {key[0]};
  MeshTopologyWireframeAnchorKey fy = {key[1]};
  MeshTopologyWireframeAnchorKey fz = {key[2]};

  uint32_t hx = fx.u * 73856093u;
  uint32_t hy = fy.u * 19349663u;
  uint32_t hz = fz.u * 83492791u;

  return (hx ^ hy ^ hz) % capacity;
}

/**
 Go through unique edges set add populate temp vertex & index array.
 Also add the newly creater index to the anchor array.
 */
void mesh_topology_wireframe_create_points(
    EdgeHashSet *edges, MeshTopologyWireframeAnchorList *anchor_list,
    MeshTopology *src_topo, MeshTopologyWireframe *dest_topo) {
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

    mesh_topology_wireframe_anchor_list_insert(
        &dest_topo->anchors, &base_vertex.position, temp_base_index, 2);

    // append opp anchor
    vindex_t temp_opp_index[] = {
        dest_topo->index.entries[index_len - 5],
        dest_topo->index.entries[index_len - 4],
    };

    mesh_topology_wireframe_anchor_list_insert(
        &dest_topo->anchors, &opp_vertex.position, temp_opp_index, 2);
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
  list->entries = calloc(capacity, sizeof(MeshTopologyWireframeAnchor));

  if (list->entries == NULL) {
    perror("Couldn't create new line mesh anchor list\n");
    list->capacity = 0;
    return MESH_TOPOLOGY_WIREFRAME_ALLOC_FAIL;
  }

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

/**
   Insert a COPY of the anchor in the anchor list.
   If the anchor already exists it appends the anchor's indexes int he existing
   item.
 */
int mesh_topology_wireframe_anchor_list_insert(
    MeshTopologyWireframeAnchorList *list, vertex_position *position,
    vindex_t *index, size_t length) {

  // create list if no entries
  if (list->entries == NULL) {
    int create_list = mesh_topology_wireframe_anchor_list_create(
        list, MESH_TOPOLOGY_WIREFRAME_ANCHOR_LIST_DEFAULT_CAPACITY);
    if (create_list != MESH_TOPOLOGY_WIREFRAME_SUCCESS)
      return MESH_TOPOLOGY_WIREFRAME_ERROR;
  }

  MeshTopologyWireframeAnchor *existing_anchor =
      mesh_topology_wireframe_anchor_list_find(list, position);

  if (existing_anchor != NULL) {
    mesh_topology_wireframe_anchor_insert(existing_anchor, index, 2);

    // printf("existing:\n");
    // mesh_topology_wireframe_anchor_print(existing_anchor);

  } else {

    // create a new anchor and insert values
    MeshTopologyWireframeAnchor *new_anchor =
        mesh_topology_wireframe_anchor_list_new(list, position);

    if (new_anchor != NULL) {
      // create anchor with given anchor
      mesh_topology_wireframe_anchor_create(
          new_anchor, MESH_TOPOLOGY_WIREFRAME_ANCHOR_DEFAULT_CAPACITY);

      // insert index in new anchor
      mesh_topology_wireframe_anchor_insert(new_anchor, index, length);

      /*DELETEME:*/
      // printf("new:\n");
      // mesh_topology_wireframe_anchor_print(new_anchor);

    } else {
      perror("Couldn't add new anchor in wireframe anchor list.\n");
      return MESH_TOPOLOGY_WIREFRAME_ERROR;
    }
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
                                          size_t capacity) {

  anchor->capacity = capacity;
  anchor->length = 0;
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

  // create anchor list
  mesh_topology_wireframe_anchor_list_create(
      &dest_topo->anchors,
      MESH_TOPOLOGY_WIREFRAME_ANCHOR_LIST_DEFAULT_CAPACITY);

  // create points from unique edges
  mesh_topology_wireframe_create_points(&edges, &dest_topo->anchors, src_topo,
                                        dest_topo);

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

MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor(MeshTopologyWireframe *wireframe,
                               const vertex_position *position) {

  return NULL;
}

VertexAttribute *
mesh_topology_wireframe_anchor_attribute(MeshTopologyWireframe *mesh,
                                         const vindex_t anchor_index) {

  return NULL;
}

void mesh_topology_wireframe_anchor_print(MeshTopologyWireframeAnchor *anchor) {

  for (size_t i = 0; i < anchor->length; i++)
    printf("%u, ", anchor->entries[i]);

  printf("\n");
}

void mesh_topology_wireframe_anchor_set_attribute(MeshTopologyWireframe *mesh,
                                                  const vindex_t anchor_index,
                                                  const VertexAttribute *va) {}

/**
   Insert new index in a given anchor.
 */
int mesh_topology_wireframe_anchor_insert(MeshTopologyWireframeAnchor *anchor,
                                          vindex_t *index, size_t length) {

  // TODO: create global list grow/create functions
  // check anchor entries capacity
  if (anchor->capacity < anchor->length + length &&
      mesh_topology_wireframe_anchor_expand(anchor) !=
          MESH_TOPOLOGY_WIREFRAME_SUCCESS) {
    perror("Couldn't allocate memory for wireframe anchor.\n");
    return MESH_TOPOLOGY_WIREFRAME_ALLOC_FAIL;
  }

  memcpy(&anchor->entries[anchor->length], index, sizeof(vindex_t) * length);
  anchor->length += length;

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}

/**
   Create a new anchor at the given index
 */
MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor_list_new(MeshTopologyWireframeAnchorList *list,
                                        vertex_position *key) {

  // check capacity
  if (list->length >= list->capacity * 0.75 &&
      mesh_topology_wireframe_anchor_list_expand(list) !=
          MESH_TOPOLOGY_WIREFRAME_SUCCESS) {
    perror("Couldn't expand wireframe anchor list.\n");
    return NULL;
  }

  uint32_t hash =
      mesh_topology_wireframe_anchor_list_hash(*key, list->capacity);
  list->length++;

  return &list->entries[hash];
}

/**
   Traverse Anchor List and compare anchor's to check if the anchor already
   exist.
 */
MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor_list_find(MeshTopologyWireframeAnchorList *list,
                                         vertex_position *position) {

  uint32_t hash =
      mesh_topology_wireframe_anchor_list_hash(*position, list->capacity);
  if (list->entries[hash].entries != NULL)
    return &list->entries[hash];

  return NULL;
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
    MeshTopologyWireframeAnchor *anchor =
        mesh_topology_wireframe_anchor_list_find(&dest_topo->anchors,
                                                 &(vec3){
                                                     base_vertex[0],
                                                     base_vertex[1],
                                                     base_vertex[2],
                                                 });

    printf("index: %u\n", base_index);
    // adjust anchor's linked index attributes
    if (anchor != NULL) {
      for (size_t w = 0; w < anchor->length; w++) {
        vindex_t wireframe_index = anchor->entries[w];
        vattr_t *wireframe_vertex =
            &dest_topo->attribute.entries[wireframe_index * VERTEX_STRIDE];

        printf("[%u] %f %f %f > ", wireframe_index, wireframe_vertex[0],
               wireframe_vertex[1], wireframe_vertex[2]);

        // update wireframe vertex position
        memcpy(wireframe_vertex, base_vertex, sizeof(vertex_position));

        printf("%f %f %f\n", wireframe_vertex[0], wireframe_vertex[1],
               wireframe_vertex[2]);
      }
    } else {
      return MESH_TOPOLOGY_WIREFRAME_ANCHOR_UNSET;
    }
  }

  // update buffer or use map_write for direct link with CPU
  wgpuQueueWriteBuffer(*queue, dest_topo->attribute.buffer, 0,
                       dest_topo->attribute.entries,
                       dest_topo->attribute.length * sizeof(vattr_t));

  return MESH_TOPOLOGY_WIREFRAME_SUCCESS;
}
