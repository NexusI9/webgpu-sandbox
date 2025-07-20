#include "editor.h"
#include "../../gizmo/gizmo.h"
#include "./selection.h"

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

  scene->editor.gizmo.grid =
      scene_new_mesh_fixed(scene, SCENE_LAYER_GIZMO_UNSELECTABLE);

  gizmo_grid_create(scene->editor.gizmo.grid,
                    &(GizmoGridCreateDescriptor){
                        .device = scene->device,
                        .queue = scene->queue,
                        .uniform =
                            (GizmoGridUniform){
                                .size = 100.0f,
                                .cell_size = 100.0f,
                                .thickness = 44.0f,
                                .color = {0.5f, 0.5f, 0.5f, 1.0f},
                            },
                    });
}

/**
   Create transform gizmos and add them to editor gizmo
 */
void scene_editor_gizmo_create_transform(Scene *scene) {

  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  gizmo_transform_create(gizmo, &(GizmoCreateDescriptor){
                                    .camera = scene->active_camera,
                                    .device = scene->device,
                                    .queue = scene->queue,
                                    .viewport = &scene->viewport,
                                    .list = &scene->meshes,
                                });

}

/**
   Return the scene editor gizmo list.
   Used when adding lights or camera into the scene.
 */
GizmoList *scene_editor_gizmo_list(Scene *scene) {
  return &scene->editor.gizmo.list;
}
