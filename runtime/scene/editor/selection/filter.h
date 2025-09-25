#ifndef _SCENE_EDITOR_SELECTION_FILTER_H_
#define _SCENE_EDITOR_SELECTION_FILTER_H_

#include <stdbool.h>
#include <stddef.h>

#include "backend/logger.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/core.h"
#include "utils/dyli.h"

typedef enum {
  SceneSelectionFilterStatus_Success,
  SceneSelectionFilterStatus_MeshAlreadySelected,
  SceneSelectionFilterStatus_MeshUnfound,
  SceneSelectionFilterStatus_UndefError,
} SceneSelectionFilterStatus;

/* Filters */
SceneSelectionFilter *scene_selection_filter_find_mesh(SceneSelection *,
                                                       Mesh *);

void scene_selection_filter_set_all_active(SceneSelectionFilter *);
void scene_selection_filter_set_all_inactive(SceneSelectionFilter *);

void scene_selection_filter_transfert_active_targets(SceneSelectionFilter *);

static inline DynamicListStatus
scene_selection_filter_selection_empty(SceneSelectionFilter *filter) {
  return dyli_empty((void *)filter->selection.entries,
                    &filter->selection.length, sizeof(SceneSelectionObject));
}

static inline SceneSelectionObject *
scene_selection_filter_selection_find_mesh(SceneSelectionFilter *filter,
                                           Mesh *mesh, size_t *index) {

  for (size_t i = 0; i < filter->selection.length; i++)
    if (filter->selection.entries[i].mesh == mesh) {
      if (index)
        *index = i;
      return &filter->selection.entries[i];
    }

  if (index)
    *index = DYLI_INVALID_INDEX;

  return NULL;
}

/**
   Create a SceneSelectionObject from the mesh and linked target and insert it
   to the filter selection list.
 */
static inline SceneSelectionFilterStatus
scene_selection_filter_selection_add_mesh(SceneSelectionFilter *filter,
                                          Mesh *mesh,
                                          const size_t *attr_index) {

  {
    // check if already selected
    SceneSelectionObject *selected_object =
        scene_selection_filter_selection_find_mesh(filter, mesh, NULL);

    if (selected_object != NULL)
      return SceneSelectionFilterStatus_MeshAlreadySelected;
  }

  size_t index = MESH_REF_LIST_UNFOUND_ENTRY;

  if (attr_index == NULL) {
    mesh_ref_list_find(&filter->meshes, mesh, &index);
  } else {
    index = *attr_index;
  }

  if (index == MESH_REF_LIST_UNFOUND_ENTRY) {
    logger_add(LoggerFlag_Warning,
               "Selection mesh not found within the selection filter.");
    return SceneSelectionFilterStatus_MeshUnfound;
  } else if (index > filter->targets.length) {
    logger_add(LoggerFlag_Warning,
               "Selection mesh index is superior to target length.");
    return SceneSelectionFilterStatus_MeshUnfound;
  }

  SceneSelectionObjectList *selection = &filter->selection;
  scene_selection_target_t *target = &filter->targets.entries[index];

  SceneSelectionObject object = {
      .initial_attribute = 0,
      .mesh = mesh,
      .target = target,
  };

  DynamicListStatus insert =
      dyli_insert((void **)&selection->entries, &selection->capacity,
                  &selection->length, sizeof(SceneSelectionObject),
                  (void *)&object, 1, "Scene Selection Object List");

  for (size_t i = 0; i < mesh->children.length; i++)
    scene_selection_filter_selection_add_mesh(filter, mesh->children.entries[i],
                                              NULL);

  return insert == DynamicListStatus_Success
             ? SceneSelectionFilterStatus_Success
             : SceneSelectionFilterStatus_UndefError;
}

static inline SceneSelectionFilterStatus
scene_selection_filter_selection_remove_mesh(SceneSelectionFilter *filter,
                                             Mesh *mesh) {

  // find mesh index
  size_t index;
  SceneSelectionObject *selected_object =
      scene_selection_filter_selection_find_mesh(filter, mesh, &index);

  if (index == DYLI_INVALID_INDEX)
    return SceneSelectionFilterStatus_MeshUnfound;

  SceneSelectionObjectList *selection_list = &filter->selection;

  DynamicListStatus remove = dyli_remove_at_index(
      (void **)&selection_list->entries, &selection_list->length,
      sizeof(SceneSelectionObject), index, "Scene Selection Object List");

  return remove == DynamicListStatus_Success
             ? SceneSelectionFilterStatus_Success
             : SceneSelectionFilterStatus_UndefError;
}

#endif
