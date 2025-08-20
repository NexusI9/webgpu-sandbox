#ifndef _VERTEX_TRANSFORM_H_
#define _VERTEX_TRANSFORM_H_

#include "attribute.h"
#include "core.h"
#include "group.h"
#include "index.h"
#include <cglm/cglm.h>

void vertex_transform_set_scale(const VertexGroup *, VertexAttribute *, vec3 *);

void vertex_transform_set_position(const VertexGroup *, VertexAttribute *,
                                   vec3 *);

void vertex_transform_set_rotation(const VertexGroup *, VertexAttribute *,
                                   vec3 *);

#endif
