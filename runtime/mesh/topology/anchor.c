#include "anchor.h"
#include "string.h"

/**
     ▗▄▖ ▗▖  ▗▖ ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▖
    ▐▌ ▐▌▐▛▚▖▐▌▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌
    ▐▛▀▜▌▐▌ ▝▜▌▐▌   ▐▛▀▜▌▐▌ ▐▌▐▛▀▚▖
    ▐▌ ▐▌▐▌  ▐▌▝▚▄▄▖▐▌ ▐▌▝▚▄▞▘▐▌ ▐▌

 */

void mesh_topology_anchor_print(MeshTopologyAnchor *anchor) {

  for (size_t i = 0; i < anchor->length; i++)
    printf("%u, ", anchor->entries[i]);

  printf("\n");
}

void mesh_topology_anchor_set_attribute(MeshTopology *mesh,
                                        const vindex_t anchor_index,
                                        const VertexAttribute *va) {}

/**
   Insert new index in a given anchor.
 */
int mesh_topology_anchor_insert(MeshTopologyAnchor *anchor, vindex_t *index,
                                size_t length) {

  // TODO: create global list grow/create functions
  // check anchor entries capacity
  if (anchor->capacity < anchor->length + length &&
      mesh_topology_anchor_expand(anchor) != MESH_TOPOLOGY_ANCHOR_SUCCESS) {
    perror("Couldn't allocate memory for wireframe anchor.\n");
    return MESH_TOPOLOGY_ANCHOR_ALLOC_FAIL;
  }

  // check if index doesn't already exists
  for (int i = 0; i < length; i++) {
    bool exists = false;
    for (size_t j = 0; j < anchor->length; j++)
      if (index[i] == anchor->entries[j]) {
        exists = true;
        break;
      }

    if (exists == false)
      anchor->entries[anchor->length++] = index[i];
  }

  return MESH_TOPOLOGY_ANCHOR_SUCCESS;
}

int mesh_topology_anchor_expand(MeshTopologyAnchor *anchor) {

  size_t new_capacity = 2 * anchor->capacity;
  vindex_t *temp =
      (vindex_t *)realloc(anchor->entries, sizeof(vindex_t) * new_capacity);

  if (temp == NULL) {
    perror("Couldn't expand line mesh anchor list\n");
    return MESH_TOPOLOGY_ANCHOR_ALLOC_FAIL;
  }

  anchor->entries = temp;
  anchor->capacity = new_capacity;

  return MESH_TOPOLOGY_ANCHOR_SUCCESS;
}

int mesh_topology_anchor_create(MeshTopologyAnchor *anchor, size_t capacity) {

  anchor->capacity = capacity;
  anchor->length = 0;
  anchor->entries = malloc(sizeof(vindex_t) * capacity);

  if (anchor->entries == NULL) {
    perror("Couldn't create new line mesh anchor\n");
    anchor->capacity = 0;
    return MESH_TOPOLOGY_ANCHOR_ALLOC_FAIL;
  }

  return MESH_TOPOLOGY_ANCHOR_SUCCESS;
}

void mesh_topology_anchor_merge(MeshTopologyAnchorList * list,
                                const VertexIndexSelection * index,
                                MeshTopologyAnchor * dest){
    
    //retrieve anchor's indexes
    for(size_t i = 0; i < index->length; i++){
	MeshTopologyAnchor* anchor = &list->entries[i];
	if(anchor){
	    
	}
    }
}

/**
     ▗▄▖ ▗▖  ▗▖ ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▖     ▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
    ▐▌ ▐▌▐▛▚▖▐▌▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌    ▐▌     █  ▐▌     █
    ▐▛▀▜▌▐▌ ▝▜▌▐▌   ▐▛▀▜▌▐▌ ▐▌▐▛▀▚▖    ▐▌     █   ▝▀▚▖  █
    ▐▌ ▐▌▐▌  ▐▌▝▚▄▄▖▐▌ ▐▌▝▚▄▞▘▐▌ ▐▌    ▐▙▄▄▖▗▄█▄▖▗▄▄▞▘  █

 */

uint32_t mesh_topology_anchor_list_hash_float(float f) {

  MeshTopologyAnchorKey conv = {f};
  uint32_t x = conv.u;

  // Murmur3
  x ^= x >> 16;
  x *= 0x85ebca6b;
  x ^= x >> 13;
  x *= 0xc2b2ae35;
  x ^= x >> 16;
  return x;
}

uint32_t mesh_topology_anchor_list_hash(vertex_position key) {

  uint32_t hx = mesh_topology_anchor_list_hash_float(key[0]);
  uint32_t hy = mesh_topology_anchor_list_hash_float(key[1]);
  uint32_t hz = mesh_topology_anchor_list_hash_float(key[2]);

  uint32_t h = hx;
  h ^= hy + 0x9e3779b9 + (h << 6) + (h >> 2); // Boost::hash_combine
  h ^= hz + 0x9e3779b9 + (h << 6) + (h >> 2);

  return h;
}

int mesh_topology_anchor_list_expand(MeshTopologyAnchorList *list) {

  size_t new_capacity = 2 * list->capacity;
  MeshTopologyAnchor *temp = (MeshTopologyAnchor *)realloc(
      list->entries, sizeof(MeshTopologyAnchor) * new_capacity);

  if (temp == NULL) {
    perror("Couldn't expand line mesh anchor list\n");
    return MESH_TOPOLOGY_ANCHOR_ALLOC_FAIL;
  }

  list->entries = temp;
  list->capacity = new_capacity;

  return MESH_TOPOLOGY_ANCHOR_SUCCESS;
}

int mesh_topology_anchor_list_create(MeshTopologyAnchorList *list,
                                     size_t capacity) {

  list->capacity = capacity;
  list->length = 0;
  list->entries = calloc(capacity, sizeof(MeshTopologyAnchor));

  if (list->entries == NULL) {
    perror("Couldn't create new line mesh anchor list\n");
    list->capacity = 0;
    return MESH_TOPOLOGY_ANCHOR_ALLOC_FAIL;
  }

  return MESH_TOPOLOGY_ANCHOR_SUCCESS;
}

/**
   Insert a COPY of the anchor in the anchor list.
   If the anchor already exists it appends the anchor's indexes int he existing
   item.
 */
int mesh_topology_anchor_list_insert(MeshTopologyAnchorList *list,
                                     vertex_position *position, vindex_t *index,
                                     size_t length) {

  // create list if no entries
  if (list->entries == NULL) {
    int create_list = mesh_topology_anchor_list_create(
        list, MESH_TOPOLOGY_ANCHOR_LIST_DEFAULT_CAPACITY);
    if (create_list != MESH_TOPOLOGY_ANCHOR_SUCCESS)
      return MESH_TOPOLOGY_ANCHOR_ERROR;
  }

  MeshTopologyAnchor *existing_anchor =
      mesh_topology_anchor_list_find_hash(list, position);

  if (existing_anchor != NULL) {
    mesh_topology_anchor_insert(existing_anchor, index, 2);

    // printf("existing:\n");
    // mesh_topology_anchor_print(existing_anchor);

  } else {

    // create a new anchor and insert values based on hashed position
    MeshTopologyAnchor *new_anchor =
        mesh_topology_anchor_list_new_hash(list, position);

    if (new_anchor != NULL) {
      // create anchor with given anchor
      mesh_topology_anchor_create(new_anchor,
                                  MESH_TOPOLOGY_ANCHOR_DEFAULT_CAPACITY);

      // insert index in new anchor
      mesh_topology_anchor_insert(new_anchor, index, length);

      /*DELETEME:*/
      // printf("new:\n");
      // mesh_topology_anchor_print(new_anchor);

    } else {
      perror("Couldn't add new anchor in wireframe anchor list.\n");
      return MESH_TOPOLOGY_ANCHOR_ERROR;
    }
  }

  return MESH_TOPOLOGY_ANCHOR_SUCCESS;
}

/**
   Create a new anchor at the given key hash
 */
MeshTopologyAnchor *
mesh_topology_anchor_list_new_hash(MeshTopologyAnchorList *list,
                                   vertex_position *key) {

  // check capacity
  if (list->length >= list->capacity * 0.75 &&
      mesh_topology_anchor_list_expand(list) != MESH_TOPOLOGY_ANCHOR_SUCCESS) {
    perror("Couldn't expand wireframe anchor list.\n");
    return NULL;
  }

  size_t hash = mesh_topology_anchor_list_hash(*key) % list->capacity;
  list->length++;

  return &list->entries[hash];
}

/**
   Create a new anchor at the given index
 */
MeshTopologyAnchor *
mesh_topology_anchor_list_new_index(MeshTopologyAnchorList *list,
                                    vindex_t index) {

  // check capacity
  if (list->capacity < index &&
      mesh_topology_anchor_list_expand(list) != MESH_TOPOLOGY_ANCHOR_SUCCESS) {
    perror("Couldn't expand wireframe anchor list.\n");
    return NULL;
  }

  list->length++;

  return &list->entries[index];
}

/**
   Get hash position and output relative entry or null if not initialised
 */
MeshTopologyAnchor *
mesh_topology_anchor_list_find_hash(MeshTopologyAnchorList *list,
                                    vertex_position *position) {

  size_t hash = mesh_topology_anchor_list_hash(*position) % list->capacity;

  if (list->entries[hash].entries != NULL)
    return &list->entries[hash];

  return NULL;
}

void mesh_topology_anchor_list_destroy(MeshTopologyAnchorList *list) {

  // clear anchors
  for (size_t i = 0; i < list->length; i++) {
    MeshTopologyAnchor *anchor = &list->entries[i];
    free(anchor->entries);
    anchor->entries = NULL;
    anchor->length = 0;
    anchor->capacity = 0;
  }

  // clear list
  free(list->entries);
  list->entries = NULL;
  list->entries = 0;
  list->capacity = 0;
}
