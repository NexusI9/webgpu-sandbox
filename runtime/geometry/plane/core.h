#ifndef _PLANE_CORE_H_
#define _PLANE_CORE_H_

#include <cglm/cglm.h>

typedef struct {
  vec3 normal;
  float d;
} InfinitePlane;

void inf_plane_create(InfinitePlane *, vec3, vec3);

#endif
