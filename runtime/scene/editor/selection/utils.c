#include "./utils.h"
#include "../../show.h"
/**
   Add mesh to the selection list
 */
void scene_selection_add(Scene *scene, Mesh *mesh) {

  // only add if mesh not already exists
  if (mesh_ref_list_find(&scene->pipelines[ScenePipeline_Fixed_Selection],
                         mesh) == NULL)
    mesh_ref_list_insert(&scene->pipelines[ScenePipeline_Fixed_Selection],
                         mesh);
}

/**
   Remove mesh from the selection.
 */
void scene_selection_remove(Scene *scene, Mesh *mesh) {

  mesh_ref_list_remove(&scene->pipelines[ScenePipeline_Fixed_Selection], mesh);
}

/**
   Set the gizmo active handle to NULL which acts as a trigger.
   This wall the loop callback doesn't move the meshes anymore if the mouse is
   down again.
 */
bool scene_selection_reset_callback(int eventType,
                                    const EmscriptenMouseEvent *mouseEvent,
                                    void *userData) {

  GizmoTransform *gizmo = (GizmoTransform *)userData;
  gizmo_transform_clear_active(gizmo);

  return EM_FALSE;
}

void scene_gizmo_transform_show(Scene *scene) {
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      &scene->pipelines[ScenePipeline_Fixed_Selection];
  scene_show_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                           ScenePipeline_Fixed_Front);
}

void scene_gizmo_transform_hide(Scene *scene) {
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      &scene->pipelines[ScenePipeline_Fixed_Selection];
  scene_hide_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                           ScenePipeline_Fixed_Front);
}

/**
   Get the selection average position (used to translate the gizmo)
 */
void scene_selection_average_position(Scene *scene, vec3 *dest) {

  MeshRefList *selection = &scene->pipelines[ScenePipeline_Fixed_Selection];

  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, *dest);

  if (selection->length == 0)
    return;

  for (size_t i = 0; i < selection->length; i++)
    glm_vec3_add(selection->entries[i]->position, *dest, *dest);

  glm_vec3_scale(*dest, 1.0f / selection->length, *dest);
}

void scene_gizmo_transform_pos_to_selection(Scene *scene) {
  
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  
  // get average position
  vec3 position;
  scene_selection_average_position(scene, &position);
  gizmo_transform_translate(gizmo, position);
}
