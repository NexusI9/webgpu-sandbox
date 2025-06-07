#include "mesh.h"

// anchor
static int line_mesh_anchor_create(LineMeshAnchor *, size_t, vindex_t);
static int line_mesh_anchor_expand(LineMeshAnchor *);

// anchor list
static int line_mesh_anchor_list_expand(LineMeshAnchorList *);
static int line_mesh_anchor_list_create(LineMeshAnchorList *, size_t);

int line_mesh_anchor_list_expand(LineMeshAnchorList *list) {

  size_t new_capacity = 2 * list->capacity;
  LineMeshAnchor *temp = (LineMeshAnchor *)realloc(
      list->entries, sizeof(LineMeshAnchor) * new_capacity);

  if (temp == NULL) {
    perror("Couldn't expand line mesh anchor list\n");
    return LINE_MESH_ALLOC_FAIL;
  }

  list->entries = temp;
  list->capacity = new_capacity;

  return LINE_MESH_SUCCESS;
}

int line_mesh_anchor_list_create(LineMeshAnchorList *list, size_t capacity) {

  list->capacity = capacity;
  list->length = 0;
  list->entries = malloc(sizeof(LineMeshAnchor) * capacity);

  if (list->entries == NULL) {
    perror("Couldn't create new line mesh anchor list\n");
    list->capacity = 0;
    return LINE_MESH_ALLOC_FAIL;
  }

  return LINE_MESH_SUCCESS;
}

int line_mesh_anchor_expand(LineMeshAnchor *anchor) {

  size_t new_capacity = 2 * anchor->capacity;
  vindex_t *temp =
      (vindex_t *)realloc(anchor->entries, sizeof(vindex_t) * new_capacity);

  if (temp == NULL) {
    perror("Couldn't expand line mesh anchor list\n");
    return LINE_MESH_ALLOC_FAIL;
  }

  anchor->entries = temp;
  anchor->capacity = new_capacity;

  return LINE_MESH_SUCCESS;
}

int line_mesh_anchor_create(LineMeshAnchor *anchor, size_t capacity,
                            vindex_t index) {

  anchor->capacity = capacity;
  anchor->length = 0;
  anchor->anchor = index;
  anchor->entries = malloc(sizeof(vindex_t) * capacity);

  if (anchor->entries == NULL) {
    perror("Couldn't create new line mesh anchor\n");
    anchor->capacity = 0;
    return LINE_MESH_ALLOC_FAIL;
  }

  return LINE_MESH_SUCCESS;
}

int line_mesh_create(LineMesh *mesh, const LineMeshCreateDescriptor *desc) {

  // link vertex attributes + index pointers
  mesh->attribute = desc->attribute;
  mesh->index = desc->index;

  // create anchor list
  line_mesh_anchor_list_create(&mesh->anchors,
                               LINE_MESH_ANCHOR_LIST_DEFAULT_CAPACITY);

  return LINE_MESH_SUCCESS;
}

LineMeshAnchor *line_mesh_anchor(LineMesh *mesh, const vindex_t anchor_index) {

  return NULL;
}

VertexAttribute *line_mesh_anchor_attribute(LineMesh *mesh,
                                            const vindex_t anchor_index) {

  return NULL;
}

void line_mesh_anchor_set_attribute(LineMesh *mesh, const vindex_t anchor_index,
                                    const VertexAttribute *va) {}

int line_mesh_anchor_insert(LineMeshAnchor *anchor, vindex_t index) {

  return LINE_MESH_SUCCESS;
}

int line_mesh_anchor_list_insert(LineMeshAnchorList *list,
                                 LineMeshAnchor *anchor) {

  return LINE_MESH_SUCCESS;
}

LineMeshAnchor *line_mesh_anchor_list_new(LineMeshAnchorList *list) {
  return NULL;
}
