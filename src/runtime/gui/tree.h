#ifndef _GUI_TREE_H_
#define _GUI_TREE_H_

#include "backend/registry.h"
#include "utils/dyli.h"

#define GUI_TREE_CAPACITY 128

typedef struct {
  reg_id_t *entries;
  size_t length;
  size_t capacity;
} GUITree;

EXTERN_C_BEGIN

static inline DynamicListStatus gui_tree_create(GUITree *tree, size_t capacity) {
  return dyli_create((void **)&tree->entries, &tree->capacity, &tree->length,
                     sizeof(reg_id_t), capacity, "Scene editor UI tree");
}

static inline reg_id_t *gui_tree_insert(GUITree *tree, reg_id_t entry) {
  if (dyli_insert((void **)&tree->entries, &tree->capacity, &tree->length,
                  sizeof(reg_id_t), (void *)&entry, 1,
                  "Scene editor UI tree") != DynamicListStatus_Success)
    return NULL;

  return tree->entries + (tree->length - 1);
}

static inline reg_id_t *gui_tree_new_entry(GUITree *tree) {
  return (reg_id_t *)dyli_new_entry((void **)&tree->entries, &tree->capacity,
                                    &tree->length, sizeof(reg_id_t),
                                    "Scene editor UI tree");
}

static inline DynamicListStatus gui_tree_empty(GUITree *tree) {
  return dyli_empty((void *)tree->entries, &tree->length, sizeof(reg_id_t));
}

static inline DynamicListStatus gui_tree_free(GUITree *tree) {
  return dyli_free((void **)&tree->entries, &tree->capacity, &tree->length);
}

static inline DynamicListStatus gui_tree_append(const GUITree *src,
                                               GUITree *dest) {
  return dyli_append((void *)src->entries, src->length, (void **)dest->entries,
                     &dest->capacity, &dest->length, sizeof(reg_id_t),
                     "Scene editor UI tree");
}

static inline DynamicListStatus gui_tree_remove_at_index(GUITree *tree,
                                                        size_t index) {
  return dyli_remove_at_index((void *)tree->entries, &tree->length,
                              sizeof(reg_id_t), index, "Scene editor UI tree");
}

EXTERN_C_END

#endif
