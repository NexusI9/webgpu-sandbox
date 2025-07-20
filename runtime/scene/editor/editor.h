#ifndef _SCENE_EDITOR_H_
#define _SCENE_EDITOR_H_

#include "../core.h"

void scene_editor_init(Scene *);
void scene_editor_gizmo_create_grid(Scene *);
void scene_editor_gizmo_create_transform(Scene *);
GizmoList *scene_editor_gizmo_list(Scene *);

#endif
