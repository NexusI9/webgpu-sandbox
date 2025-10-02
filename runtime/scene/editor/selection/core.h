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

void scene_selection_average_position(SceneSelection *, vec3 *);

size_t scene_selection_length(SceneSelection *);

void scene_selection_subscribe_mesh_ref_list(SceneSelection *, MeshRefList *,
                                             void *, const SceneSelectionType);

void scene_selection_subscribe_mesh(SceneSelection *, Mesh *, void *,
                                    const SceneSelectionType);

void scene_selection_toggle_mesh(Scene *, Mesh *);

void scene_selection_empty(SceneSelection *);
void scene_selection_all(SceneSelection *);
void scene_selection_cache_initial_attributes(SceneSelection *,
                                              const GizmoMode);
void scene_selection_clear_initial_attributes(SceneSelection *);

/**
   To transform the selected meshes and gizmo we poll the mouse event and check
   if the selection pipeline has length.

   Basically our camera raycast/ html events are only used to:
     1. push/pop mesh from the selection array (on right click)
     2. update the gizmo transform active axis (on left click)

    We then constantly through the loop:
     1. check if the mouse is pressed

    According to those checkes we then transform the meshes.

 */
static inline void scene_selection_draw_callback(void *data) {

  Scene *scene = (Scene *)data;
  SceneSelection *selection = &scene->editor.selection;
  Gizmo *gizmo = &scene->editor.gizmo.transform;

  if (gizmo->cache.init_distance != 0.0f) {

    // use each selection filters transform callbacks on their respective meshes
    for (int i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {

      vec3 delta;

      // 1. transform gizmo
      gizmo_transform_callback gizmo_transform_callback =
          gizmo->transform_callback[gizmo->mode];

      gizmo_transform_callback(gizmo, scene->active_camera, &scene->viewport,
                               &delta);

      // 2. transform filter selection with delta calculated by gizmo
      SceneSelectionFilter *filter = &selection->filters[i];

      // look-up filter transform callback depending on gizmo mode
      // (loc/rot/scale)
      scene_selection_transform_callback mesh_transform_callback =
          filter->transform_callback;

      SceneSelectionTransform transform = {
          .selection = &filter->selection,
          .delta = &delta,
          .axis = gizmo->axis,
          .transform_mode = gizmo->mode,
          .scene = scene,
      };
      mesh_transform_callback(&transform);

      gizmo_update_ssbo(gizmo, &scene->renderer.ssbo);
    }
  }
}

#ifdef __cplusplus
}
#endif

#endif
