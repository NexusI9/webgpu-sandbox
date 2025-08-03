#ifndef _SCENE_EDITOR_SELECTION_UTILS_H_
#define _SCENE_EDITOR_SELECTION_UTILS_H_

#include "../../core.h"

/* Gizmo utils */
void scene_gizmo_transform_pos_to_selection(GizmoTransform *, SceneSelection *);
void scene_gizmo_transform_show(Scene *);
void scene_gizmo_transform_hide(Scene *);

#endif
