#ifndef _SCENE_EDITOR_H_
#define _SCENE_EDITOR_H_

#include "../core.h"
#include "object/list/list.h"

void scene_editor_init(Scene *);

SceneEditorObjectList *scene_editor_object_list(Scene *);

#endif
