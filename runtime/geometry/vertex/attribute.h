#ifndef _VERTEX_ATTRIBUTE_H_
#define _VERTEX_ATTRIBUTE_H_
#include <stddef.h>
#include <webgpu/webgpu.h>

#include "core.h"

typedef float vattr_t;

typedef struct {
  vattr_t *entries;
  size_t length;
  size_t capacity;
  WGPUBuffer buffer;
} VertexAttribute;

void vertex_attribute_find_equal_attr(Vertex *, VertexAttribute *,
                                      VertexAttributeType, VertexAttribute *);
void vertex_attribute_print(VertexAttribute *);

void vertex_attribute_set_position(VertexAttribute *, const vertex_position);
void vertex_attribute_set_normal(VertexAttribute *, const vertex_normal);
void vertex_attribute_set_color(VertexAttribute *, const vertex_color);
void vertex_attribute_set_uv(VertexAttribute *, const vertex_uv);

void vertex_attribute_set_position_add(VertexAttribute *,
                                       const vertex_position);
void vertex_attribute_set_normal_add(VertexAttribute *, const vertex_normal);
void vertex_attribute_set_color_add(VertexAttribute *, const vertex_color);
void vertex_attribute_set_uv_add(VertexAttribute *, const vertex_uv);

VertexStatus vertex_attribute_copy(VertexAttribute *, VertexAttribute *);
void vertex_attribute_destroy(VertexAttribute *);

#endif
