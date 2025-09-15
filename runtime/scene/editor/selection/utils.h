#ifndef _SCENE_EDITOR_SELECTION_UTILS_H_
#define _SCENE_EDITOR_SELECTION_UTILS_H_

#include "../../core.h"

/* Gizmo utils */
void scene_gizmo_pos_to_selection(Gizmo *, SceneSelection *, SSBOManager *);
void scene_gizmo_show(Scene *);
void scene_gizmo_hide(Scene *);

#endif
