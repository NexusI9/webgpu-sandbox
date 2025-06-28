#ifndef _CAMERA_RAYCAST_H_
#define _CAMERA_RAYCAST_H_

#include "core.h"
#include "../mesh/mesh.h"

typedef struct {
  em_mouse_callback_func callback;
  void *data;
} CameraRaycastEventDescriptor;

typedef struct {

  const char *target;
  CameraRaycastEventDescriptor hover;
  CameraRaycastEventDescriptor click;

} CameraRaycastDescriptor;


void camera_raycast_center(Camera *, const CameraRaycastDescriptor *);
void camera_raycast_mouse(Camera *, const CameraRaycastDescriptor *);

void camera_raycast_on_hit(Camera*, MeshRefList**, size_t);

#endif
