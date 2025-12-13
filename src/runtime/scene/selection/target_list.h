#ifndef _SCENE_EDITOR_SELECTION_TARGET_LIST_H_
#define _SCENE_EDITOR_SELECTION_TARGET_LIST_H_

#include <stddef.h>

#include "backend/registry.h"
#include "runtime/scene/core.h"
#include "utils/dyli.h"

#include "backend/registry.h"
#include "runtime/scene/core.h"
#include "utils/dyli.h"

static inline DynamicListStatus
scene_selection_target_list_create(SceneSelectionTargetList *list,
                                   size_t capacity) {

  return dyli_create((void **)&list->entries, &list->capacity, &list->count,
                     sizeof(reg_id_t), capacity, "Scene selection target list");
}

static inline reg_id_t *
scene_selection_target_list_insert(SceneSelectionTargetList *list,
                                   reg_id_t entry) {

  if (dyli_insert((void **)&list->entries, &list->capacity, &list->count,
                  sizeof(reg_id_t), (void *)&entry, 1,
                  "Scene selection target list") != DynamicListStatus_Success)
    return NULL;

  return &list->entries[list->count-1];
}

static inline reg_id_t *
scene_selection_target_list_new_entry(SceneSelectionTargetList *list) {
  return (reg_id_t *)dyli_new_entry((void **)&list->entries, &list->capacity,
                                    &list->count, sizeof(reg_id_t),
                                    "Scene selection target list");
}

static inline DynamicListStatus
scene_selection_target_list_empty(SceneSelectionTargetList *list) {
  return dyli_empty((void *)list->entries, &list->count, sizeof(reg_id_t));
}

static inline DynamicListStatus
scene_selection_target_list_free(SceneSelectionTargetList *list) {
  return dyli_free((void **)&list->entries, &list->capacity, &list->count);
}

static inline DynamicListStatus
scene_selection_target_list_append(const SceneSelectionTargetList *src,
                                   SceneSelectionTargetList *dest) {

  return dyli_append((void *)src->entries, src->count, (void **)dest->entries,
                     &dest->capacity, &dest->count, sizeof(reg_id_t),
                     "Scene selection target list");
}

static inline DynamicListStatus
scene_selection_target_remove_at_index(SceneSelectionTargetList *list,
                                       size_t index) {
  return dyli_remove_at_index((void *)list->entries, &list->count,
                              sizeof(reg_id_t), index,
                              "Scene selection target list");
}

#endif
