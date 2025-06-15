#ifndef _VERTEX_TRANSFORM_H_
#define _VERTEX_TRANSFORM_H_

#include "attribute.h"
#include "core.h"
#include "index.h"
#include <cglm/cglm.h>

void vertex_transform_scale(const VertexIndex *, VertexAttribute *, vec3 *);
void vertex_transform_translate(const VertexIndex *, VertexAttribute *, vec3 *);
void vertex_transform_rotate(const VertexIndex *, VertexAttribute *, vec3 *);

void vertex_transform_alike_by_position(const VertexIndex *,
                                        const VertexAttribute *,
                                        vertex_position *, VertexIndex *);
void vertex_transform_alike_by_index(const VertexIndex *,
                                     const VertexAttribute *, vindex_t,
                                     VertexIndex *);

#endif
