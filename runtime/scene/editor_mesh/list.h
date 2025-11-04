#ifndef _SCENE_EDTIOR_OBJECT_LIST_H_
#define _SCENE_EDTIOR_OBJECT_LIST_H_

#include <stddef.h>

#include "backend/registry.h"
#include "runtime/scene/core.h"
#include "runtime/scene/scene.h"
#include "utils/dyli.h"
#include "utils/name.h"

/*
  GIZMO LIST
  Gizmos' scene entities (meshes) are spearated from their data (primary
  struct). As a result gizmos meshes and data are gathered under the
  GizmoList struct.

      Mesh Pool
     .----------.
     |  Mesh 1  | -----------.
     |----------|            |      Gizmo List
     |  Mesh 2  | -----------|   .-------------.
     |----------|            '-> |  meshes [*] |
     |  Mesh n  |                |  length 2   |
     '----------'                |  -          |
                             .-> |  target *   |
      Gizmo Pool            |    '-------------'
     .-----------.          |
     | Camera 1  | ---------'
     |-----------|
     | Camera n  |
     '-----------'

 */

/* === SEM LIST === */

static inline DynamicListStatus sem_list_create(SceneEditorMeshList *, size_t,
                                                const char *,
                                                const RegEntryType);
static inline SceneEditorMesh *sem_list_insert(SceneEditorMeshList *,
                                               SceneEditorMesh *);
static inline SceneEditorMesh *sem_list_new_entry(SceneEditorMeshList *);
static inline DynamicListStatus sem_list_remove(SceneEditorMeshList *,
                                                SceneEditorMesh *);
static inline DynamicListStatus sem_list_destroy(SceneEditorMeshList *);

static inline const char *sem_list_get_name(SceneEditorMeshList *list) {
  return list->name;
}

static inline void sem_list_set_name(SceneEditorMeshList *list,
                                     const char *name) {
  name_copy(name, list->name);
}

DynamicListStatus sem_list_create(SceneEditorMeshList *list, size_t capacity,
                                  const char *name, const RegEntryType type) {

  list->id = reg_register(reg_new_id(), list, type);
  sem_list_set_name(list, name == 0 ? "Scene Editor Mesh List" : name);

  return dyli_create((void **)&list->entries, &list->capacity, &list->length,
                     sizeof(SceneEditorMesh), capacity,
                     "Scene Editor Mesh list");
}

SceneEditorMesh *sem_list_insert(SceneEditorMeshList *list,
                                 SceneEditorMesh *entry) {

  if (dyli_insert((void **)&list->entries, &list->capacity, &list->length,
                  sizeof(SceneEditorMesh), (void *)entry, 1,
                  "Scene Editor Mesh list") != DynamicListStatus_Success)
    return NULL;

  return entry;
}

SceneEditorMesh *sem_list_new_entry(SceneEditorMeshList *list) {

  SceneEditorMesh *entry = (SceneEditorMesh *)dyli_new_entry(
      (void **)&list->entries, &list->capacity, &list->length,
      sizeof(SceneEditorMesh), "Scene Editor Mesh list");

  if (entry)
    entry->id = reg_register(reg_new_id(), entry, RegEntryType_SceneEditorMesh);

  return entry;
}

DynamicListStatus sem_list_remove(SceneEditorMeshList *list,
                                  SceneEditorMesh *entry) {
  return dyli_remove((void *)list->entries, &list->length,
                     sizeof(SceneEditorMesh), (void *)entry,
                     "Scene Editor Mesh list");
}

DynamicListStatus sem_list_destroy(SceneEditorMeshList *list) {
  return dyli_free((void **)list->entries, &list->capacity, &list->capacity);
}

static inline SceneEditorMesh *sem_list_get_origin(SceneEditorMeshList *list) {
  return &list->entries[SEM_LIST_ORIGIN_INDEX];
}

/* === SEM LIST ARRAY === */

static inline DynamicListStatus
sem_list_array_create(SceneEditorMeshListArray *array, size_t capacity) {
  return dyli_create((void **)&array->entries, &array->capacity, &array->length,
                     sizeof(SceneEditorMeshList), capacity,
                     "Scene Editor Mesh List Array");
}

static inline SceneEditorMeshList *
sem_list_array_new_entry(SceneEditorMeshListArray *array,
                         const RegEntryType type) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)dyli_new_entry(
      (void **)&array->entries, &array->capacity, &array->length,
      sizeof(SceneEditorMeshList), "Scene Editor Mesh List Array");

  return list;
}

static inline DynamicListStatus
sem_list_array_remove(SceneEditorMeshListArray *array,
                      SceneEditorMeshList *entry) {
  return dyli_remove((void *)array->entries, &array->length,
                     sizeof(SceneEditorMeshList), (void *)entry,
                     "Scene Editor Mesh List Array");
}

static inline DynamicListStatus
sem_list_array_destroy(SceneEditorMeshListArray *array) {
  return dyli_free((void **)&array->entries, &array->capacity,
                   &array->capacity);
}

#endif
