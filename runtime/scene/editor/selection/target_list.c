#include "target_list.h"

#include "runtime/scene/core.h"
#include "utils/dyli.h"

DynamicListStatus
scene_selection_target_list_create(SceneSelectionTargetList *list,
                                   size_t capacity) {

  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(scene_selection_target_t), capacity,
                     "Scene selection target list");
}

scene_selection_target_t
scene_selection_target_list_insert(SceneSelectionTargetList *list,
                                   scene_selection_target_t entry) {

  if (dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                  sizeof(scene_selection_target_t), (void *)&entry, 1,
                  "Scene selection target list") != DynamicListStatus_Success)
    return NULL;

  return entry;
}

scene_selection_target_t
scene_selection_target_list_new_entry(SceneSelectionTargetList *list) {
  return (scene_selection_target_t)dyli_new_entry(
      (void *)&list->entries, &list->capacity, &list->length,
      sizeof(scene_selection_target_t), "Scene selection target list");
}

DynamicListStatus
scene_selection_target_list_empty(SceneSelectionTargetList *list) {
  return dyli_empty((void *)list->entries, &list->length,
                    sizeof(scene_selection_target_t));
}

DynamicListStatus
scene_selection_target_list_free(SceneSelectionTargetList *list) {
  return dyli_free((void *)&list->entries, &list->capacity, &list->length);
}

DynamicListStatus
scene_selection_target_list_append(const SceneSelectionTargetList *src,
                                      SceneSelectionTargetList *dest) {

  return dyli_append((void *)src->entries, src->length,
                        (void *)&dest->entries, &dest->capacity, &dest->length,
                        sizeof(scene_selection_target_t),
                        "Scene selection target list");
}

DynamicListStatus
scene_selection_target_remove_at_index(SceneSelectionTargetList *list,
                                       size_t index) {
  return dyli_remove_at_index((void *)list->entries, &list->length,
                              sizeof(scene_selection_target_t), index,
                              "Scene selection target list");
}
