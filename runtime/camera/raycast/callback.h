#ifndef _CAMERA_RAYCAST_CALLBACK_H_
#define _CAMERA_RAYCAST_CALLBACK_H_

#include "../../raycast/raycast.h"
#include "../../viewport/viewport.h"
#include "../core.h"
#include "./hit_list.h"

bool camera_raycast_event_callback_center(int, const EmscriptenMouseEvent *,
                                          void *);
bool camera_raycast_event_callback_mouse(int, const EmscriptenMouseEvent *,
                                         void *);

bool camera_raycast_event_destructor(void *);
#endif
