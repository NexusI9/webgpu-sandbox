#ifndef _AABB_H_
#define _AABB_H_

#include <cglm/cglm.h>
#include <cglm/types.h>
#include <cglm/vec3.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "runtime/geometry/vertex/vertex.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/attribute.h"

// Axis-Aligned Bounding Box
typedef struct {
  vec3 min;
  vec3 max;
} AABB;

bool aabb_within_distance(const AABB *, const AABB *, const float, float *);
void aabb_from_vec3(AABB *, vec3 *, const size_t);
void aabb_from_vert_attr(AABB *, const VertexAttribute *);

void aabb_corners(AABB *, vec3[8]);
void aabb_to_worldspace(AABB *, vec3[8], mat4);

static inline bool aabb_intersect(const AABB *a, const AABB *b) {

  for (uint8_t i = 0; i < 3; i++)
    if (a->min[i] > b->max[i] || a->max[i] < b->min[i])
      return false;

  return true;
}

static inline void aabb_copy(AABB *src, AABB *dest) {
  glm_vec3_copy(src->max, dest->max);
  glm_vec3_copy(src->min, dest->min);
}

#endif
