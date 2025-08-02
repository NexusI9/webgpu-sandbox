#include "editor.h"
#include "../../gizmo/gizmo.h"
#include "../scene.h"
#include "./selection/selection.h"
#include <stddef.h>

static inline void scene_editor_gizmo_create_grid(Scene *);
static inline void scene_editor_gizmo_create_transform(Scene *);

/**
   Initialize scene editor main elements such as: grid, gizmo list allocation,
   and scene selection.
 */
void scene_editor_init(Scene *scene) {

  // init editor related gizmos
  scene_editor_gizmo_create_grid(scene);

  // init transform gizmo
  scene_editor_gizmo_create_transform(scene);

  //  init gizmo list
  gizmo_list_create(scene_editor_gizmo_list(scene),
                    GIZMO_LIST_CAPACITY_DEFAULT);

  // init selection list & related events
  scene_selection_init(scene);
}

/**
   Create grid and assign it to the editor gizmo
 */
void scene_editor_gizmo_create_grid(Scene *scene) {

  scene->editor.gizmo.grid = scene_new_mesh(scene);

  gizmo_grid_create(scene->editor.gizmo.grid,
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

  scene_add_mesh(scene, scene->editor.gizmo.grid, ScenePipeline_Fixed,
                 SCENE_LAYER_UNSELECTABLE);
}

/**
   Create transform gizmos and add them to editor gizmo
 */
void scene_editor_gizmo_create_transform(Scene *scene) {

  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  gizmo_transform_create(gizmo, &(GizmoCreateDescriptor){
                                    .camera = scene->active_camera,
                                    .device = scene_device(scene),
                                    .queue = scene_queue(scene),
                                    .viewport = &scene->viewport,
                                    .list = &scene->meshes,
                                });

  for (size_t i = 0; i < 3; i++) {
    // add the gizmo interactive handles to 'Gizmo Transform' layer as to only include this
    // layer for he raycast selection
    scene_layer_set_insert_mesh_ref_list(
        &scene->layers, SCENE_LAYER_GIZMO_TRANSFORM, &gizmo->interactive_handles[i]);

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
GizmoList *scene_editor_gizmo_list(Scene *scene) {
  return &scene->editor.gizmo.list;
}
