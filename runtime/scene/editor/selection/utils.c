#include "./utils.h"

#include <cglm/types.h>

#include "runtime/scene/show.h"
#include "./core.h"
#include "runtime/mesh/core.h"
#include "backend/ssbo.h"
#include "runtime/scene/core.h"

void scene_gizmo_show(Scene *scene) {
  Gizmo *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      scene_pipeline(scene, ScenePipeline_Fixed_Selection);

  // add to render pipeline
  scene_show_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                           ScenePipeline_Fixed_Front);
}

void scene_gizmo_hide(Scene *scene) {
  Gizmo *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      scene_pipeline(scene, ScenePipeline_Fixed_Selection);
  scene_hide_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                           ScenePipeline_Fixed_Front);
}

/**
   Get the selection average position (used to translate the gizmo).
 */
void scene_gizmo_pos_to_selection(Gizmo *gizmo,
                                            SceneSelection *selection,
                                            SSBOManager *ssbo) {
  // get average position
  vec3 position;
  scene_selection_average_position(selection, &position);
  gizmo_set_position(gizmo, position);

  // update ssbo matrix buffer
  gizmo_update_ssbo(gizmo, ssbo);
}
