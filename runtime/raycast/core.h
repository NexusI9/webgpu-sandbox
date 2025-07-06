#ifndef _RAYCAST_CORE_H_
#define _RAYCAST_CORE_H_

#include "../geometry/aabb/aabb.h"
#include "../mesh/mesh.h"
#include <cglm/cglm.h>

#define RAYCAST_DISTANCE_INF 1000.0f

typedef struct {
  vec3 origin;
  vec3 direction;
  float distance;
  bool hit;
} Raycast;

typedef struct {
  vec3 origin;
  vec3 direction;
  float distance;
} RaycastCreateDescriptor;


bool raycast_hit_aabb(Raycast *, const AABB *, float*);

#endif
