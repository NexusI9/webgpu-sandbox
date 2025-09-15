#ifndef _SCENE_EDITOR_SELECTION_FILTER_H_
#define _SCENE_EDITOR_SELECTION_FILTER_H_

#include "../runtime/scene/core.h"

typedef enum {
  SceneSelectionFilterStatus_Success,
  SceneSelectionFilterStatus_MeshAlreadySelected,
  SceneSelectionFilterStatus_MeshUnfound,
  SceneSelectionFilterStatus_UndefError,
} SceneSelectionFilterStatus;

/* Filters */
SceneSelectionFilter *scene_selection_filter_find_mesh(SceneSelection *,
                                                       Mesh *);

bool scene_selection_filter_include_mesh(SceneSelectionFilter *, Mesh *,
                                         size_t *);

void scene_selection_filter_set_all_active(SceneSelectionFilter *);
void scene_selection_filter_set_all_inactive(SceneSelectionFilter *);

void scene_selection_filter_transfert_active_targets(SceneSelectionFilter *);

SceneSelectionFilterStatus
scene_selection_filter_set_active(SceneSelectionFilter *, Mesh *);

SceneSelectionFilterStatus
scene_selection_filter_set_inactive(SceneSelectionFilter *, Mesh *);

#endif
