#ifndef _SCENE_EDITOR_SELECTION_TARGET_LIST_H_
#define _SCENE_EDITOR_SELECTION_TARGET_LIST_H_

#include <stddef.h>

#include "backend/registry.h"
#include "runtime/scene/core.h"
#include "utils/dyli.h"

/* Object List */
DynamicListStatus scene_selection_target_list_create(SceneSelectionTargetList *,
                                                     size_t);

reg_id_t* scene_selection_target_list_insert(SceneSelectionTargetList *,
                                            selection_targets);

DynamicListStatus
scene_selection_target_remove_at_index(SceneSelectionTargetList *, size_t);

DynamicListStatus
scene_selection_target_list_append(const SceneSelectionTargetList *,
                                   SceneSelectionTargetList *);

reg_id_t* scene_selection_target_list_new_entry(SceneSelectionTargetList *);

DynamicListStatus scene_selection_target_list_empty(SceneSelectionTargetList *);

DynamicListStatus scene_selection_target_list_free(SceneSelectionTargetList *);

#endif
