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

typedef struct {
  vec3 *origin;
  vec3 *target;
  vec3 *axis_direction;
  mat4 *view;
  mat4 *projection;
  float x;
  float y;
  int width;
  int height;
} RaycastProjectScreenToAxis;

bool raycast_hit_aabb(Raycast *, const AABB *, float *);
void raycast_from_screen(Raycast *, vec3 *, mat4 *, mat4 *, float, float);
void raycast_project_to_axis(Raycast *, vec3 *, vec3 *, vec3 *);

void raycast_project_from_screen_to_axis(const RaycastProjectScreenToAxis*, vec3*);
#endif
