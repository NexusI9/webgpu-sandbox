#ifndef _SCENE_EDITOR_SELECTION_FILTER_H_
#define _SCENE_EDITOR_SELECTION_FILTER_H_

#include <stdbool.h>
#include <stddef.h>

#include "backend/logger.h"
#include "backend/registry.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "utils/dyli.h"

typedef enum {
  SceneSelectionFilterStatus_Success,
  SceneSelectionFilterStatus_MeshAlreadySelected,
  SceneSelectionFilterStatus_MeshUnfound,
  SceneSelectionFilterStatus_UndefError,
} SceneSelectionFilterStatus;

typedef struct {
  Mesh *mesh;
  reg_id_t target;
  vec3 initial_attribute;
} SceneSelectionObject;

typedef struct {
  SceneSelectionObject *entries;
  size_t capacity;
  size_t count;
} SceneSelectionObjectList;

typedef struct {
  reg_id_t *entries;
  size_t count;
  size_t capacity;
} SceneSelectionTargetList;

// linked attribtutes ( mesh[i] + targets[i] => Selection Object )
typedef struct {
  MeshRefList meshes;

  SceneSelectionTargetList targets;
  SceneSelectionObjectList selection;

  void *highlight_data;
  void *transform_data;

} SceneSelectionFilter;

EXTERN_C_BEGIN

/* Filters */

void scene_selection_filter_set_all_active(SceneSelectionFilter *);
void scene_selection_filter_set_all_inactive(SceneSelectionFilter *);

static inline SceneSelectionFilterStatus
scene_selection_filter_selection_remove_mesh(SceneSelectionFilter *, Mesh *);

static inline SceneSelectionFilterStatus
scene_selection_filter_selection_add_mesh(SceneSelectionFilter *, Mesh *,
                                          const size_t *);

static inline SceneSelectionObject *
scene_selection_filter_selection_find_mesh(SceneSelectionFilter *, Mesh *,
                                           size_t *);

static inline DynamicListStatus
scene_selection_filter_selection_empty(SceneSelectionFilter *);

DynamicListStatus
scene_selection_filter_selection_empty(SceneSelectionFilter *filter) {
  return dyli_empty((void *)filter->selection.entries,
                    &filter->selection.count, sizeof(SceneSelectionObject));
}

SceneSelectionObject *
scene_selection_filter_selection_find_mesh(SceneSelectionFilter *filter,
                                           Mesh *mesh, size_t *index) {

  for (size_t i = 0; i < filter->selection.count; i++)
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

    .----------.                .---------------.
    |   Mesh   |------.         | Selection Obj |
    '----------'      |________ | + Mesh        |
    .----------.      |         | + Target      |
    |  Target  |------'         | + Init Attr   |
    '----------'                '---------------'

 */
SceneSelectionFilterStatus scene_selection_filter_selection_add_mesh(
    SceneSelectionFilter *filter, Mesh *mesh, const size_t *attr_index) {

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
  } else if (index > filter->targets.count) {
    logger_add(LoggerFlag_Warning,
               "Selection mesh index is superior to target count.");
    return SceneSelectionFilterStatus_MeshUnfound;
  }

  SceneSelectionObjectList *selection = &filter->selection;

  SceneSelectionObject object = {
      .initial_attribute = 0,
      .mesh = mesh,
      .target = filter->targets.entries[index],
  };

  DynamicListStatus insert =
      dyli_insert((void **)&selection->entries, &selection->capacity,
                  &selection->count, sizeof(SceneSelectionObject),
                  (void *)&object, 1, "Scene Selection Object List");

  for (size_t i = 0; i < mesh->children.count; i++)
    scene_selection_filter_selection_add_mesh(filter, mesh->children.entries[i],
                                              NULL);

  return insert == DynamicListStatus_Success
             ? SceneSelectionFilterStatus_Success
             : SceneSelectionFilterStatus_UndefError;
}

SceneSelectionFilterStatus
scene_selection_filter_selection_remove_mesh(SceneSelectionFilter *filter,
                                             Mesh *mesh) {

  // find mesh index
  size_t index = MESH_REF_LIST_UNFOUND_ENTRY;
  SceneSelectionObject *selected_object =
      scene_selection_filter_selection_find_mesh(filter, mesh, &index);

  if (index == DYLI_INVALID_INDEX)
    return SceneSelectionFilterStatus_MeshUnfound;

  SceneSelectionObjectList *selection_list = &filter->selection;

  DynamicListStatus remove = dyli_remove_at_index(
      (void *)selection_list->entries, &selection_list->count,
      sizeof(SceneSelectionObject), index, "Scene Selection Object List");

  for (size_t i = 0; i < mesh->children.count; i++)
    scene_selection_filter_selection_remove_mesh(filter,
                                                 mesh->children.entries[i]);

  return remove == DynamicListStatus_Success
             ? SceneSelectionFilterStatus_Success
             : SceneSelectionFilterStatus_UndefError;
}

EXTERN_C_END

#endif
