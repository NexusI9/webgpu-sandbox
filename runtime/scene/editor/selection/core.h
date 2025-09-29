#ifndef _SCENE_EDITOR_SELECTION_CORE_H_
#define _SCENE_EDITOR_SELECTION_CORE_H_

#include "runtime/scene/core.h"

#include <cglm/types.h>
#include <stddef.h>

#include "emscripten/html5.h"
#include "gizmo/core.h"
#include "runtime/mesh/core.h"

typedef struct {
  Scene *scene;
} SceneSelectionCallbackData;

#ifdef __cplusplus
extern "C" {
#endif

void scene_selection_init(Scene *);

void scene_selection_draw_callback(void *);

void scene_selection_average_position(SceneSelection *, vec3 *);

size_t scene_selection_length(SceneSelection *);

void scene_selection_subscribe_mesh_ref_list(SceneSelection *, MeshRefList *,
                                             void *, const SceneSelectionType);

void scene_selection_subscribe_mesh(SceneSelection *, Mesh *, void *,
                                    const SceneSelectionType);

void scene_selection_update_mesh(Scene *, Mesh *);
void scene_selection_disable_mesh(Scene *, Mesh *);

void scene_selection_empty(SceneSelection *);
void scene_selection_all(SceneSelection *);
void scene_selection_cache_initial_attributes(SceneSelection *,
                                              const GizmoMode);
void scene_selection_clear_initial_attributes(SceneSelection *);

#ifdef __cplusplus
}
#endif

#endif
