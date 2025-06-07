#ifndef _LINE_MESH_H_
#define _LINE_MESH_H_
#include "../vertex/vertex.h"


#define LINE_MESH_ANCHOR_LIST_DEFAULT_CAPACITY 128
#define LINE_MESH_ANCHOR_DEFAULT_CAPACITY 32
#define LINE_MESH_SUCCESS 0
#define LINE_MESH_ALLOC_FAIL 1
#define LINE_MESH_ERROR 2

typedef struct {
  vindex_t anchor;
  vindex_t *entries;
  size_t length;
  size_t capacity;
} LineMeshAnchor;

typedef struct {
  LineMeshAnchor *entries;
  size_t length;
  size_t capacity;
} LineMeshAnchorList;

typedef struct {
  VertexIndex *index;
  VertexAttribute *attribute;
  LineMeshAnchorList anchors;
} LineMesh;

typedef struct {
  VertexIndex *index;
  VertexAttribute *attribute;
} LineMeshCreateDescriptor;

int line_mesh_create(LineMesh *, const LineMeshCreateDescriptor *);

LineMeshAnchor *line_mesh_anchor(LineMesh *, const vindex_t);

VertexAttribute *line_mesh_anchor_attribute(LineMesh *, const vindex_t);

void line_mesh_anchor_set_attribute(LineMesh *, const vindex_t,
                                    const VertexAttribute *);

int line_mesh_anchor_insert(LineMeshAnchor *, vindex_t);
int line_mesh_anchor_list_insert(LineMeshAnchorList *, LineMeshAnchor *);
LineMeshAnchor *line_mesh_anchor_list_new(LineMeshAnchorList *);

#endif
