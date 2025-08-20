#include "./utils.h"
#include "../../show.h"
#include "./core.h"
#include "stdbool.h"
#include <stdint.h>

void scene_gizmo_transform_show(Scene *scene) {
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      scene_pipeline(scene, ScenePipeline_Fixed_Selection);
  scene_show_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                           ScenePipeline_Fixed_Front);
}

void scene_gizmo_transform_hide(Scene *scene) {
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      scene_pipeline(scene, ScenePipeline_Fixed_Selection);
  scene_hide_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                           ScenePipeline_Fixed_Front);
}

/**
   Get the selection average position (used to translate the gizmo).
 */
void scene_gizmo_transform_pos_to_selection(GizmoTransform *gizmo,
                                            SceneSelection *selection) {

  // get average position
  vec3 position;
  scene_selection_average_position(selection, &position);
  gizmo_transform_set_position(gizmo, position);
}
