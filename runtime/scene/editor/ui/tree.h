#ifndef _SCENE_EDITOR_UI_TREE_H_
#define _SCENE_EDITOR_UI_TREE_H_

#include "backend/registry.h"
#include "utils/dyli.h"

#define SCENE_EDITOR_UI_TREE_CAPACITY 128

typedef struct {
  reg_id_t *entries;
  size_t length;
  size_t capacity;
} SceneEditorUITree;

EXTERN_C_BEGIN

static inline DynamicListStatus
scene_editor_ui_tree_create(SceneEditorUITree *tree, size_t capacity) {
  return dyli_create((void **)&tree->entries, &tree->capacity, &tree->length,
                     sizeof(reg_id_t), capacity, "Scene editor UI tree");
}

static inline reg_id_t *scene_editor_ui_tree_insert(SceneEditorUITree *tree,
                                                    reg_id_t entry) {
  if (dyli_insert((void **)&tree->entries, &tree->capacity, &tree->length,
                  sizeof(reg_id_t), (void *)&entry, 1,
                  "Scene editor UI tree") != DynamicListStatus_Success)
    return NULL;

  return tree->entries + (tree->length - 1);
}

static inline reg_id_t *
scene_editor_ui_tree_new_entry(SceneEditorUITree *tree) {
  return (reg_id_t *)dyli_new_entry((void **)&tree->entries, &tree->capacity,
                                    &tree->length, sizeof(reg_id_t),
                                    "Scene editor UI tree");
}

static inline DynamicListStatus
scene_editor_ui_tree_empty(SceneEditorUITree *tree) {
  return dyli_empty((void *)tree->entries, &tree->length, sizeof(reg_id_t));
}

static inline DynamicListStatus
scene_editor_ui_tree_free(SceneEditorUITree *tree) {
  return dyli_free((void **)&tree->entries, &tree->capacity, &tree->length);
}

static inline DynamicListStatus
scene_editor_ui_tree_append(const SceneEditorUITree *src,
                            SceneEditorUITree *dest) {
  return dyli_append((void *)src->entries, src->length, (void **)dest->entries,
                     &dest->capacity, &dest->length, sizeof(reg_id_t),
                     "Scene editor UI tree");
}

static inline DynamicListStatus
scene_editor_ui_tree_remove_at_index(SceneEditorUITree *tree, size_t index) {
  return dyli_remove_at_index((void *)tree->entries, &tree->length,
                              sizeof(reg_id_t), index, "Scene editor UI tree");
}

EXTERN_C_END

#endif
