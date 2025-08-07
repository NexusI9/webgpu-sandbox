#ifndef _RAYCAST_CORE_H_
#define _RAYCAST_CORE_H_

#include "../geometry/aabb/aabb.h"
#include "../geometry/plane/plane.h"
#include "../utils/vector/vector.h"
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
  mat4 *view;
  mat4 *projection;
  vec3 *axis_direction; // 1D axis only (x,y,z)
  vec3 *target;         // 1D axis only (x,y,z)
  InfinitePlane *plane; // 2D axis only (xy,yz,xz)
  float x;
  float y;
  int width;
  int height;
} RaycastProjectScreenToAxis;

typedef void (*raycast_project_from_screen_callback)(
    Raycast *, const RaycastProjectScreenToAxis *, vec3 *);

bool raycast_hit_aabb(Raycast *, const AABB *, float *);
bool raycast_hit_inf_plane(Raycast *, InfinitePlane *, vec3 *);
void raycast_from_screen(Raycast *, vec3 *, mat4 *, mat4 *, float, float);
void raycast_project_to_axis(Raycast *, vec3 *, vec3 *, vec3 *);

void raycast_project_from_screen_to_axis(Raycast *,
                                         const RaycastProjectScreenToAxis *,
                                         vec3 *);

void raycast_project_from_screen_to_plane(Raycast *,
                                          const RaycastProjectScreenToAxis *,
                                          vec3 *);

void raycast_project_from_screen(Raycast *, const Axis,
                                 RaycastProjectScreenToAxis *, vec3 *);
#endif
