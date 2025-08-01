#ifndef _SCENE_EDITOR_SELECTION_CORE_H_
#define _SCENE_EDITOR_SELECTION_CORE_H_

#include "../../core.h"
#include "emscripten/html5.h"


typedef struct {
  Scene *scene;
} SceneSelectionCallbackData;

void scene_selection_init(Scene *);

void scene_selection_draw_callback(void *);

#endif
