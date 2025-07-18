#ifndef _CAMERA_RAYCAST_METHOD_H_
#define _CAMERA_RAYCAST_METHOD_H_

#include "../../raycast/raycast.h"
#include "../../viewport/viewport.h"
#include "../core.h"
#include "./callback.h"
#include "./hit_list.h"

typedef void (*camera_raycast_cast_method)(Raycast *, Camera *, Viewport *);

void camera_raycast_cast_method_mouse(Raycast *, Camera *, Viewport *);
void camera_raycast_cast_method_center(Raycast *, Camera *, Viewport *);
#endif
