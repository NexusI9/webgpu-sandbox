#ifndef _SCENE_EDITOR_SELECTION_H_
#define _SCENE_EDITOR_SELECTION_H_

#include "../core.h"
#include "emscripten/html5.h"

void scene_selection_init(Scene *);

void scene_selection_raycast_callback(CameraRaycastCallback *,
                                      const EmscriptenMouseEvent *, void *);

#endif
