#ifndef _RAYCAST_CORE_H_
#define _RAYCAST_CORE_H_

#include "../camera/camera.h"
#include "../scene/scene.h"
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

void raycast_camera_cursor(Raycast *, Camera *, Viewport *);
void raycast_camera_center(Raycast *, Camera *, Viewport *);
Mesh *raycast_check_mesh_list(Raycast *, const MeshRefList *);

#endif
