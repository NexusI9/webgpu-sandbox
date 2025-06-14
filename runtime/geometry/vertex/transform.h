#ifndef _VERTEX_TRANSFORM_H_
#define _VERTEX_TRANSFORM_H_

#include "attribute.h"
#include "core.h"
#include "index.h"
#include <cglm/cglm.h>

void vertex_transform_scale(const VertexIndex *, VertexAttribute *, vec3 *);
void vertex_transform_translate(const VertexIndex *, VertexAttribute *, vec3 *);
void vertex_transform_rotate(const VertexIndex *, VertexAttribute *, vec3 *);

void vertex_transform_get_by_position(const VertexIndex *, vertex_position *,
                                      VertexIndex *);
void vertex_transform_get_by_index(const VertexIndex *, vertex_position *,
                                   VertexIndex *);

#endif
