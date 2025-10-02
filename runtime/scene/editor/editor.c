#include "editor.h"

#include <stddef.h>

#include "backend/context.h"
#include "mesh/grid/grid.h"
#include "mesh/list/list.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/add.h"
#include "runtime/scene/build.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/layer.h"
#include "runtime/scene/renderer/core.h"
#include "selection/core.h"
#include "selection/gizmo/core.h"

static inline void scene_editor_gizmo_create_grid(Scene *);
static inline void scene_editor_gizmo_create_transform(Scene *);

/**
   Initialize scene editor main elements such as: grid, gizmo list allocation,
   and scene selection.
 */
void scene_editor_init(Scene *scene) {

  // init selection list & related events
  scene_selection_init(scene);

  scene_editor_ui_init(&scene->editor.ui, &(SceneEditorUIDescriptor){
                                              .clock = &scene->renderer.clock,
                                              .dpi = context_dpi(),
                                          });

  // init editor related gizmos
  scene_editor_gizmo_create_grid(scene);

  // init transform gizmo
  scene_editor_gizmo_create_transform(scene);

  //  init gizmo list
  sem_list_array_create(scene_editor_mesh_list(&scene->editor),
                        SCENE_EDITOR_OBJECT_LIST_CAPACITY_DEFAULT);
}

/**
   Create grid and assign it to the editor gizmo
 */
void scene_editor_gizmo_create_grid(Scene *scene) {

  scene->editor.gizmo.grid = scene_new_mesh(scene);

  sem_grid_create(scene->editor.gizmo.grid,
                  &(GizmoGridCreateDescriptor){
                      .uniform =
                          (GizmoGridUniform){
                              .size = 100.0f,
                              .cell_size = 100.0f,
                              .thickness = 44.0f,
                              .color = {0.5f, 0.5f, 0.5f, 1.0f},
                          },
                  });

  scene_add_mesh_fixed(scene, scene->editor.gizmo.grid, ScenePipeline_Fixed,
                       NULL, SceneAddFlag_Unselectable);
}

/**
   Create transform gizmos and add them to editor gizmo
 */
void scene_editor_gizmo_create_transform(Scene *scene) {

  Gizmo *gizmo = &scene->editor.gizmo.transform;
  gizmo_create(gizmo, &(GizmoCreateDescriptor){
                          .camera = scene->active_camera,
                          .viewport = &scene->viewport,
                          .list = &scene->meshes,
                      });

  for (size_t i = 0; i < GIZMO_MODE_COUNT; i++) {
    for (size_t j = 0; j < gizmo->handles[i].length; j++) {
      Mesh *mesh = gizmo->handles[i].entries[j];
      scene_add_mesh_fixed(scene, mesh, ScenePipeline_Fixed_Front,
                           SCENE_LAYER_GIZMO,
                           SceneAddFlag_Hide | SceneAddFlag_Unselectable);
    }
  }
}
