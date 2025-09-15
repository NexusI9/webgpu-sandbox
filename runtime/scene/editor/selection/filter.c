#include "filter.h"
#include "target_list.h"

/**
   Transfert all the filter objects to meshes[SceneSelectionState_Selected].
   */
void scene_selection_filter_set_all_active(SceneSelectionFilter *filter) {
  // empty selected meshes first
  mesh_ref_list_empty(&filter->meshes[SceneSelectionState_Selected]);

  // transfert all filter meshes as active
  mesh_ref_list_append(&filter->meshes[SceneSelectionState_Default],
                          &filter->meshes[SceneSelectionState_Selected], NULL);

  // empty active targets first
  scene_selection_target_list_empty(
      &filter->targets[SceneSelectionState_Selected]);

  // transfert all targets as active
  scene_selection_target_list_append(
      &filter->targets[SceneSelectionState_Default],
      &filter->targets[SceneSelectionState_Selected]);

  if (filter->highlight_callback)
    filter->highlight_callback(&filter->meshes[SceneSelectionState_Selected],
                               filter->highlight_data);
}

/**
   Empty all the filter objects to meshes[SceneSelectionState_Selected].
   */
void scene_selection_filter_set_all_inactive(SceneSelectionFilter *filter) {
  // empty selected meshes first
  mesh_ref_list_empty(&filter->meshes[SceneSelectionState_Selected]);

  // empty active targets first
  scene_selection_target_list_empty(
      &filter->targets[SceneSelectionState_Selected]);

  if (filter->highlight_callback)
    filter->highlight_callback(&filter->meshes[SceneSelectionState_Selected],
                               filter->highlight_data);
}

/**
   Search if a mesh belong to a scene selection filter.
   Returns the target filter or NULL if not found.
 */
SceneSelectionFilter *
scene_selection_filter_find_mesh(SceneSelection *selection, Mesh *mesh) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    if (scene_selection_filter_include_mesh(filter, mesh, NULL)) {
      return filter;
    }
  }

  return NULL;
}

/**
   Linearily look up for a mesh within a filter default mesh list
 */
bool scene_selection_filter_include_mesh(SceneSelectionFilter *filter,
                                         Mesh *mesh, size_t *index) {

  // check include
  for (size_t j = 0; j < filter->meshes[SceneSelectionState_Default].length;
       j++) {

    Mesh *find = mesh_ref_list_find(
        &filter->meshes[SceneSelectionState_Default], mesh, index);

    // if mesh found in current include list
    if (find != NULL)
      return true;
  }

  return false;
}

/**
   Add the given mesh to the filter selected array and transfert the linked
   target accordingly
 */
SceneSelectionFilterStatus
scene_selection_filter_set_active(SceneSelectionFilter *filter, Mesh *mesh) {

  // check if already selected
  Mesh *already_selected = mesh_ref_list_find(
      &filter->meshes[SceneSelectionState_Selected], mesh, NULL);

  if (already_selected != NULL)
    return SceneSelectionFilterStatus_MeshAlreadySelected;

  // find mesh index
  size_t index = -1;
  mesh_ref_list_find(&filter->meshes[SceneSelectionState_Default], mesh,
                     &index);

  if (index < 0)
    return SceneSelectionFilterStatus_MeshUnfound;

  // transfert mesh to active list
  mesh_ref_list_insert(&filter->meshes[SceneSelectionState_Selected], mesh);

  // transfert relative target to active list (may be NULL)
  scene_selection_target_t target =
      filter->targets[SceneSelectionState_Default].entries[index];

  scene_selection_target_list_insert(
      &filter->targets[SceneSelectionState_Selected], target);

  return SceneSelectionFilterStatus_Success;
}

SceneSelectionFilterStatus
scene_selection_filter_set_inactive(SceneSelectionFilter *filter, Mesh *mesh) {

  // find mesh index
  size_t index = -1;
  mesh_ref_list_find(&filter->meshes[SceneSelectionState_Selected], mesh,
                     &index);

  if (index < 0)
    return SceneSelectionFilterStatus_MeshUnfound;

  // remove mesh
  mesh_ref_list_remove_at_index(&filter->meshes[SceneSelectionState_Selected],
                                index);

  // remove linked target
  scene_selection_target_remove_at_index(
      &filter->targets[SceneSelectionState_Selected], index);

  return SceneSelectionFilterStatus_Success;
}
