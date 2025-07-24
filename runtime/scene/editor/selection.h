#ifndef _SCENE_EDITOR_SELECTION_H_
#define _SCENE_EDITOR_SELECTION_H_

#include "../core.h"
#include "emscripten/html5.h"

typedef struct {
  Scene *scene;
} SceneSelectionCallbackData;

void scene_selection_init(Scene *);

void scene_selection_raycast_right_click_callback(CameraRaycastCallback *,
                                                  const EmscriptenMouseEvent *,
                                                  void *);

void scene_selection_raycast_mouse_move_callback(CameraRaycastCallback *,
                                                 const EmscriptenMouseEvent *,
                                                 void *);
#endif
