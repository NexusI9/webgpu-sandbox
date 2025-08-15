#ifndef _AABB_H_
#define _AABB_H_

#include "../vertex/vertex.h"
#include <cglm/cglm.h>

// Axis-Aligned Bounding Box
typedef struct {
  vec3 min;
  vec3 max;
} AABB;

bool aabb_within_distance(const AABB *, const AABB *, const float, float*);
void aabb_from_vec3(AABB *, vec3 *, const size_t);
void aabb_from_vert_attr(AABB *, const VertexAttribute *);

void aabb_corners(AABB *, vec3[8]);
void aabb_to_worldspace(AABB *, vec3[8], mat4);

static inline void aabb_copy(AABB *src, AABB *dest) {
  glm_vec3_copy(src->max, dest->max);
  glm_vec3_copy(src->min, dest->min);
}

#endif
