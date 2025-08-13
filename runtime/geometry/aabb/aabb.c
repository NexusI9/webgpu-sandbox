#include "aabb.h"
#include <math.h>

bool aabb_within_distance(const AABB *a, const AABB *b, const float distance) {

  float sq = 0.0f;
  float max_distance = distance * distance;

  for (size_t i = 0; i < 3; i++) {
    float axis_distance = fmaxf(0.0f, fmaxf(a->min[i] - b->max[i], b->min[i] - a->max[i]));
    sq += axis_distance * axis_distance;

    if (sq > max_distance)
      return false;
  }

  return sq <= max_distance;
}
