#include "editor.h"

#include <stddef.h>

#include "object/grid/grid.h"
#include "object/list/list.h"
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

  scene_editor_ui_init(&scene->editor.ui,
                       &(SceneEditorUIDescriptor){
                           .height = &scene->renderer.context.height,
                           .width = &scene->renderer.context.width,
                           .dpi = &scene->renderer.context.dpi,
                           .swapchain = &scene->renderer.wgpu.swapchain,
                           .device = scene_device(scene),
                           .queue = scene_queue(scene),
                           .clock = &scene->renderer.clock,
                       });

  // init editor related gizmos
  scene_editor_gizmo_create_grid(scene);

  // init transform gizmo
  scene_editor_gizmo_create_transform(scene);

  //  init gizmo list
  seo_list_create(scene_editor_object_list(scene),
                  SCENE_EDITOR_OBJECT_LIST_CAPACITY_DEFAULT);
}

/**
   Create grid and assign it to the editor gizmo
 */
void scene_editor_gizmo_create_grid(Scene *scene) {

  scene->editor.gizmo.grid = scene_new_mesh(scene);

  seo_grid_create(scene->editor.gizmo.grid,
                  &(GizmoGridCreateDescriptor){
                      .device = scene_device(scene),
                      .queue = scene_queue(scene),
                      .uniform =
                          (GizmoGridUniform){
                              .size = 100.0f,
                              .cell_size = 100.0f,
                              .thickness = 44.0f,
                              .color = {0.5f, 0.5f, 0.5f, 1.0f},
                          },
                  });
  /*
    Do not use this function cause it adds it as selectable and cause crash
  scene_add_mesh_fixed(scene, scene->editor.gizmo.grid, ScenePipeline_Fixed,
  SCENE_LAYER_UNSELECTABLE);*/
  scene_build_mesh(scene, scene->editor.gizmo.grid, ScenePipeline_Fixed);
  mesh_ref_list_insert(scene_pipeline(scene, ScenePipeline_Fixed),
                       scene->editor.gizmo.grid);
}

/**
   Create transform gizmos and add them to editor gizmo
 */
void scene_editor_gizmo_create_transform(Scene *scene) {

  Gizmo *gizmo = &scene->editor.gizmo.transform;
  gizmo_create(gizmo, &(GizmoCreateDescriptor){
                          .camera = scene->active_camera,
                          .device = scene_device(scene),
                          .queue = scene_queue(scene),
                          .viewport = &scene->viewport,
                          .list = &scene->meshes,
                      });

  for (size_t i = 0; i < 3; i++) {
    // add the gizmo interactive handles to 'Gizmo Transform' layer as to only
    // include this layer for he raycast selection
    scene_layer_set_insert_mesh_ref_list(&scene->layers, SCENE_LAYER_GIZMO,
                                         &gizmo->interactive_handles[i]);

    // build each guizmo mode mesh ref list
    // we do not "Add" them, only Build cause we don't necessarily want to show
    // them unless meshes have been selected.
    scene_build_mesh_ref_list(scene, &gizmo->handles[i],
                              ScenePipeline_Fixed_Front);
  }
}

/**
   Return the scene editor gizmo list.
   Used when adding lights or camera into the scene.
 */
SceneEditorObjectList *scene_editor_object_list(Scene *scene) {
  return &scene->editor.seo_list;
}
