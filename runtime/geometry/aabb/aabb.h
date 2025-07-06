#ifndef _AABB_H_
#define _AABB_H_

#include <cglm/cglm.h>

// Axis-Aligned Bounding Box
typedef struct {
  vec3 min;
  vec3 max;
} AABB;

#endif
