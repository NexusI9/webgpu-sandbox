#ifndef _SCENE_EDITOR_SELECTION_CORE_H_
#define _SCENE_EDITOR_SELECTION_CORE_H_

#include "../../core.h"
#include "emscripten/html5.h"

typedef struct {
  Scene *scene;
} SceneSelectionCallbackData;

void scene_selection_init(Scene *);

void scene_selection_draw_callback(void *);

void scene_selection_add(MeshRefList *, Mesh *);

void scene_selection_average_position(SceneSelection *, vec3 *);

void scene_selection_meshes_lists(SceneSelection *,
                                  MeshRefList *[SCENE_SELECTION_TYPE_COUNT],
                                  size_t *);

size_t scene_selection_length(SceneSelection *);

void scene_selection_add_mesh_ref_list(SceneSelection *, MeshRefList *, void *,
                                       const SceneSelectionType);

void scene_selection_add_mesh(SceneSelection *selection, Mesh *list,
                              void *extra, const SceneSelectionType type);

void scene_selection_empty(SceneSelection *);
void scene_selection_all(SceneSelection *);
void scene_selection_cache_initial_attributes(SceneSelection *,
                                              const GizmoTransformMode);
void scene_selection_empty_initial_attributes(SceneSelection *);

#endif
