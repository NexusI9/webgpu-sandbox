#ifndef _CAMERA_RAYCAST_UTILS_H_
#define _CAMERA_RAYCAST_UTILS_H_

#include <stdbool.h>

#include "../camera.h"
#include "../runtime/camera/core.h"
#include "../runtime/geometry/aabb/aabb.h"
#include "../runtime/mesh/core.h"
#include "../runtime/mesh/ref_list.h"
#include "../runtime/camera/core.h"
#include "../runtime/geometry/aabb/aabb.h"
#include "../runtime/mesh/core.h"
#include "../runtime/mesh/ref_list.h"

void camera_raycast_screen_space(Camera *, Mesh *, float, AABB *);

bool camera_raycast_is_excluded(const MeshRefListArray *, Mesh *);

#endif
