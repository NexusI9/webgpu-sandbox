#include "aabb.h"

#include <math.h>
#include <cglm/mat4.h>
#include <float.h>

#include "../runtime/geometry/vertex/core.h"

bool aabb_within_distance(const AABB *a, const AABB *b, const float distance,
                          float *dest) {

  float sq = 0.0f;
  float max_distance = distance * distance;

  for (size_t i = 0; i < 3; i++) {
    float axis_distance =
        fmaxf(0.0f, fmaxf(a->min[i] - b->max[i], b->min[i] - a->max[i]));
    sq += axis_distance * axis_distance;

    if (dest)
      *dest = sq;

    if (sq > max_distance)
      return false;
  }

  return sq <= max_distance;
}

void aabb_from_vec3(AABB *bound, vec3 *pos, const size_t length) {

  glm_vec3_copy((vec3){FLT_MAX, FLT_MAX, FLT_MAX}, bound->min);
  glm_vec3_copy((vec3){-FLT_MAX, -FLT_MAX, -FLT_MAX}, bound->max);

  for (size_t i = 0; i < length; i++) {
    glm_vec3_minv(bound->min, pos[i], bound->min);
    glm_vec3_maxv(bound->max, pos[i], bound->max);
  }
}

void aabb_from_vert_attr(AABB *bound, const VertexAttribute *va) {

  glm_vec3_copy((vec3){FLT_MAX, FLT_MAX, FLT_MAX}, bound->min);
  glm_vec3_copy((vec3){-FLT_MAX, -FLT_MAX, -FLT_MAX}, bound->max);

  for (size_t i = 0; i < va->length; i += VERTEX_STRIDE) {
    vattr_t *current = &va->entries[i];
    glm_vec3_minv(bound->min, current, bound->min);
    glm_vec3_maxv(bound->max, current, bound->max);
  }
}

/**
  Compute 8 corners of bound

        6---------7
       /|        /|
      / |       / |
     2--+------3  |
     |  |      |  |
     |  4------+--5
     | /       | /
     0---------1'

 */
void aabb_corners(AABB *bound, vec3 corners[8]) {
  glm_vec3_copy((vec3){bound->min[0], bound->min[1], bound->min[2]},
                corners[0]);
  glm_vec3_copy((vec3){bound->max[0], bound->min[1], bound->min[2]},
                corners[1]);
  glm_vec3_copy((vec3){bound->min[0], bound->max[1], bound->min[2]},
                corners[2]);
  glm_vec3_copy((vec3){bound->max[0], bound->max[1], bound->min[2]},
                corners[3]);

  glm_vec3_copy((vec3){bound->min[0], bound->min[1], bound->max[2]},
                corners[4]);
  glm_vec3_copy((vec3){bound->max[0], bound->min[1], bound->max[2]},
                corners[5]);
  glm_vec3_copy((vec3){bound->min[0], bound->max[1], bound->max[2]},
                corners[6]);
  glm_vec3_copy((vec3){bound->max[0], bound->max[1], bound->max[2]},
                corners[7]);
}

/**
   Transform the local space bound to world space based on model matrix
 */
void aabb_to_worldspace(AABB *bound, vec3 corners[8], mat4 matrix) {

  // transforms corners with model matrix
  glm_mat4_mulv3(matrix, corners[0], 1.0f, bound->min); // init min
  glm_vec3_copy(bound->min, bound->max);                // init max

  // compare
  vec3 transformed;
  for (int i = 0; i < 8; i++) {
    glm_mat4_mulv3(matrix, corners[i], 1.0f, transformed);
    glm_vec3_minv(bound->min, transformed, bound->min);
    glm_vec3_maxv(bound->max, transformed, bound->max);
  }
}
