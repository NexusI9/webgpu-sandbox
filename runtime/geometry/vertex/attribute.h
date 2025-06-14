#ifndef _VERTEX_ATTRIBUTE_H_
#define _VERTEX_ATTRIBUTE_H_
#include "core.h"

typedef float vattr_t;

typedef struct {
  vattr_t *entries;
  size_t length;
  size_t capacity;
  WGPUBuffer buffer;
} VertexAttribute;

void vertex_attribute_find_equal_attr(Vertex *, VertexAttribute *,
                                      VertexAttributeName, VertexAttribute *);
void vertex_attribute_print(VertexAttribute *);

void vertex_attribute_set_position(VertexAttribute *, vertex_position *);
void vertex_attribute_set_normal(VertexAttribute *, vertex_normal *);
void vertex_attribute_set_color(VertexAttribute *, vertex_color *);
void vertex_attribute_set_uv(VertexAttribute *, vertex_uv *);
int vertex_attribute_copy(VertexAttribute *, VertexAttribute *);
void vertex_attribute_destroy(VertexAttribute *);

#endif
