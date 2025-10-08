#ifndef _SCENE_EDTIOR_OBJECT_LIST_H_
#define _SCENE_EDTIOR_OBJECT_LIST_H_

#include <stddef.h>

#include "backend/registry.h"
#include "runtime/scene/core.h"
#include "runtime/scene/scene.h"
#include "utils/dyli.h"

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

static inline DynamicListStatus sem_list_create(SceneEditorMeshList *, size_t);
static inline SceneEditorMesh *sem_list_insert(SceneEditorMeshList *,
                                               SceneEditorMesh *);
static inline SceneEditorMesh *sem_list_new_entry(SceneEditorMeshList *);
static inline DynamicListStatus sem_list_remove(SceneEditorMeshList *,
                                                SceneEditorMesh *);
static inline DynamicListStatus sem_list_destroy(SceneEditorMeshList *);

DynamicListStatus sem_list_create(SceneEditorMeshList *list, size_t capacity) {
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
    entry->id = reg_register(entry, RegEntryType_SceneEditorMesh);

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

/* === SEM LIST ARRAY === */

static inline DynamicListStatus
sem_list_array_create(SceneEditorMeshListArray *, size_t);

static inline SceneEditorMeshList *
sem_list_array_new_entry(SceneEditorMeshListArray *);

static inline DynamicListStatus
sem_list_array_remove(SceneEditorMeshListArray *, SceneEditorMeshList *);

static inline DynamicListStatus
sem_list_array_destroy(SceneEditorMeshListArray *);

DynamicListStatus sem_list_array_create(SceneEditorMeshListArray *array,
                                        size_t capacity) {
  return dyli_create((void **)&array->entries, &array->capacity, &array->length,
                     sizeof(SceneEditorMeshList), capacity,
                     "Scene Editor Mesh List Array");
}

SceneEditorMeshList *sem_list_array_new_entry(SceneEditorMeshListArray *array) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)dyli_new_entry(
      (void **)&array->entries, &array->capacity, &array->length,
      sizeof(SceneEditorMeshList), "Scene Editor Mesh List Array");

  list->id = reg_register(list, RegEntryType_SceneEditorMeshList);

  return list;
}

DynamicListStatus sem_list_array_remove(SceneEditorMeshListArray *array,
                                        SceneEditorMeshList *entry) {
  return dyli_remove((void *)array->entries, &array->length,
                     sizeof(SceneEditorMeshList), (void *)entry,
                     "Scene Editor Mesh List Array");
}

DynamicListStatus sem_list_array_destroy(SceneEditorMeshListArray *array) {
  return dyli_free((void **)&array->entries, &array->capacity,
                   &array->capacity);
}

#endif
