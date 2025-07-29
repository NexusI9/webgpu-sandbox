#ifndef _CAMERA_RAYCAST_UTILS_H_
#define _CAMERA_RAYCAST_UTILS_H_

#include "../camera.h"

void camera_raycast_screen_space(Camera *, Mesh *, float, AABB *);

bool camera_raycast_is_excluded(const MeshRefListArray *, Mesh *);

#endif
