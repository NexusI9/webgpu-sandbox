#ifndef _SCENE_EDITOR_SELECTION_UTILS_H_
#define _SCENE_EDITOR_SELECTION_UTILS_H_

#include "../../core.h"

/* Scene utils */
void scene_selection_add(MeshRefList *, Mesh *);

/* Scene Selection */
void scene_selection_average_position(SceneSelection *, vec3 *);
void scene_selection_meshes_lists(SceneSelection *,
                                  MeshRefList *[SCENE_SELECTION_TYPE_COUNT],
                                  size_t *);
size_t scene_selection_length(SceneSelection *);
SceneSelectionFilter *scene_selection_filter_find_mesh(SceneSelection *,
                                                       Mesh *);

bool scene_selection_filter_include_mesh(SceneSelectionFilter *, Mesh *);
void scene_selection_filter_add_mesh(SceneSelectionFilter*, Mesh*);
void scene_selection_empty(SceneSelection *);

/* Gizmo utils */
void scene_gizmo_transform_pos_to_selection(GizmoTransform *, SceneSelection *);
void scene_gizmo_transform_show(Scene *);
void scene_gizmo_transform_hide(Scene *);

#endif
