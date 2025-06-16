#ifndef _VERTEX_TRANSFORM_H_
#define _VERTEX_TRANSFORM_H_

#include "attribute.h"
#include "core.h"
#include "index.h"
#include <cglm/cglm.h>
#include "group.h"

void vertex_transform_scale(const VertexGroup *, VertexAttribute *,
                            vec3 *);

void vertex_transform_translate(const VertexGroup *, VertexAttribute *,
                                vec3 *);

void vertex_transform_rotate(const VertexGroup *, VertexAttribute *,
                             vec3 *);

#endif
