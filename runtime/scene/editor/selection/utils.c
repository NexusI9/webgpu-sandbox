#include "./utils.h"
#include "../../show.h"
#include <stdint.h>
/**
   Add mesh to the selection list
 */
void scene_selection_add(MeshRefList *list, Mesh *mesh) {

  // only add if mesh not already exists
  if (mesh_ref_list_find(list, mesh) == NULL)
    mesh_ref_list_insert(list, mesh);
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
void scene_gizmo_transform_pos_to_selection(GizmoTransform *gizmo,
                                            SceneSelectionSet *selection) {

  // get average position
  vec3 position;

  MeshRefList *list[SCENE_SELECTION_TYPE_COUNT];
  size_t length;
  scene_selection_meshes_lists(selection, list, &length);

  uint8_t denom = 0;

  for (size_t i = 0; i < length; i++) {
    vec3 list_avg;
    mesh_ref_list_average_position(list[i], &list_avg);
    glm_vec3_add(position, list_avg, position);
    if (list[i]->length > 0)
      denom++;
  }

  glm_vec3_scale(position, 1.0f / glm_max(denom, 1), position);

  gizmo_transform_translate(gizmo, position);
}

void scene_selection_meshes_lists(SceneSelectionSet *selection,
                                  MeshRefList *list[SCENE_SELECTION_TYPE_COUNT],
                                  size_t *length) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    list[i] = &selection[i].source;

  *length = SCENE_SELECTION_TYPE_COUNT;
}

size_t scene_selection_length(SceneSelectionSet *selection) {

  size_t length = 0;
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    length += selection[i].source.length;

  return length;
}
