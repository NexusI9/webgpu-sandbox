#ifndef _SCENE_EDITOR_SELECTION_TARGET_LIST_H_
#define _SCENE_EDITOR_SELECTION_TARGET_LIST_H_

#include "../runtime/scene/core.h"

/* Object List */
DynamicListStatus scene_selection_target_list_create(SceneSelectionTargetList *,
                                                     size_t);

scene_selection_target_t
scene_selection_target_list_insert(SceneSelectionTargetList *,
                                   scene_selection_target_t);

DynamicListStatus scene_selection_target_remove_at_index(SceneSelectionTargetList*, size_t);

DynamicListStatus scene_selection_target_list_transfert(const SceneSelectionTargetList *,
                                           SceneSelectionTargetList *);

scene_selection_target_t
scene_selection_target_list_new_entry(SceneSelectionTargetList *);

DynamicListStatus scene_selection_target_list_empty(SceneSelectionTargetList *);

DynamicListStatus scene_selection_target_list_free(SceneSelectionTargetList *);

#endif
