#ifndef _SCENE_EDITOR_SELECTION_UTILS_H_
#define _SCENE_EDITOR_SELECTION_UTILS_H_

#include "../../core.h"

/* Scene utils */
void scene_selection_add(MeshRefList *, Mesh *);
void scene_selection_average_position(Scene *, vec3 *);

/* Gizmo utils */
void scene_gizmo_transform_pos_to_selection(GizmoTransform *, MeshRefList *);
void scene_gizmo_transform_show(Scene *);
void scene_gizmo_transform_hide(Scene *);

#endif
