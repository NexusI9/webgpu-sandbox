#include "list.h"

DynamicListStatus seo_list_create(SceneEditorObjectList *list, size_t capacity) {
  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(SceneEditorObject), capacity,
                     "Scene Editor Object list");
}

SceneEditorObject *seo_list_insert(SceneEditorObjectList *list,
                                   SceneEditorObject *entry) {

  if (dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                  sizeof(SceneEditorObject), (void *)&entry, 1,
                  "Scene Editor Object list") != DynamicListStatus_Success)
    return NULL;

  return entry;
}

SceneEditorObject *seo_list_new_entry(SceneEditorObjectList *list) {
  SceneEditorObject new_entry;
  seo_list_insert(list, &new_entry);
  return &list->entries[list->length];
}
