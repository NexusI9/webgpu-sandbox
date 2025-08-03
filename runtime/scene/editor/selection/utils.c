#include "./utils.h"
#include "../../show.h"
#include "stdbool.h"
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
   Get the average position of all selected mesh in all filters.
 */
void scene_selection_average_position(SceneSelection *selection, vec3 *dest) {

  glm_vec3_zero(*dest);

  uint8_t denom = 0;

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    vec3 filter_avg;
    mesh_ref_list_average_position(&filter->selection, &filter_avg);
    glm_vec3_add(*dest, filter_avg, *dest);

    if (filter->selection.length > 0)
      denom++;
  }

  glm_vec3_scale(*dest, 1.0f / glm_max(denom, 1), *dest);
}

/**
   Get the selection average position (used to translate the gizmo).
 */
void scene_gizmo_transform_pos_to_selection(GizmoTransform *gizmo,
                                            SceneSelection *selection) {

  // get average position
  vec3 position;
  scene_selection_average_position(selection, &position);
  gizmo_transform_translate(gizmo, position);
}

void scene_selection_meshes_lists(SceneSelection *selection,
                                  MeshRefList *list[SCENE_SELECTION_TYPE_COUNT],
                                  size_t *length) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    list[i] = &selection[i].filters->selection;

  *length = SCENE_SELECTION_TYPE_COUNT;
}

size_t scene_selection_length(SceneSelection *selection) {

  size_t length = 0;
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    length += selection->filters[i].selection.length;

  return length;
}

bool scene_selection_filter_include_mesh(SceneSelectionFilter *filter,
                                         Mesh *mesh) {
  bool included = false;
  // check include
  for (size_t j = 0; j < filter->include.length; j++) {

    MeshRefList *include_list = filter->include.entries[j];

    Mesh *find = mesh_ref_list_find(include_list, mesh);

    // if mesh found in current include list
    if (find != NULL) {
      included = true;

      // if no exclude, no need to check anymore
      if (filter->exclude.length == 0)
        break;
    }
  }

  // check exclude
  for (size_t k = 0; k < filter->exclude.length; k++) {
    MeshRefList *exclude_list = filter->exclude.entries[k];

    // cancel if mesh is actually excluded from the filter
    if (mesh_ref_list_find(exclude_list, mesh) != NULL)
      included = false;
  }

  return included;
}

/**
   Search if a mesh belong to a scene selection filter.
   Returns the target filter or NULL if not found.
 */
SceneSelectionFilter *
scene_selection_filter_find_mesh(SceneSelection *selection, Mesh *mesh) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    if (scene_selection_filter_include_mesh(filter, mesh))
      return filter;
  }

  return NULL;
}

void scene_selection_filter_add_mesh(SceneSelectionFilter *filter, Mesh *mesh) {
  mesh_ref_list_insert(&filter->selection, mesh);

  // transftert (optional)
  if (filter->transfert)
    mesh_ref_list_insert(filter->transfert, mesh);
}

/**
   Empty each filter's selection
 */
void scene_selection_empty(SceneSelection *selection) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    mesh_ref_list_empty(&filter->selection);

    if (filter->transfert)
      mesh_ref_list_empty(filter->transfert);
  }
}
