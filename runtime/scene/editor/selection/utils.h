#ifndef _SCENE_EDITOR_SELECTION_UTILS_H_
#define _SCENE_EDITOR_SELECTION_UTILS_H_

#include "../../core.h"

/* Scene utils */
void scene_selection_add(MeshRefList *, Mesh *);
void scene_selection_average_position(Scene *, vec3 *);
void scene_selection_meshes_lists(SceneSelectionSet *,
                                  MeshRefList *[SCENE_SELECTION_TYPE_COUNT],
                                  size_t *);
size_t scene_selection_length(SceneSelectionSet *);

/* Gizmo utils */
void scene_gizmo_transform_pos_to_selection(GizmoTransform *,
                                            SceneSelectionSet *);
void scene_gizmo_transform_show(Scene *);
void scene_gizmo_transform_hide(Scene *);

#endif
