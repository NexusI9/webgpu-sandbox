#include "list.h"

#include "runtime/scene/core.h"
#include "utils/dyli.h"

DynamicListStatus seo_list_create(SceneEditorObjectList *list,
                                  size_t capacity) {
  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(SceneEditorObject), capacity,
                     "Scene Editor Object list");
}

SceneEditorObject *seo_list_insert(SceneEditorObjectList *list,
                                   SceneEditorObject *entry) {

  if (dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                  sizeof(SceneEditorObject), (void *)entry, 1,
                  "Scene Editor Object list") != DynamicListStatus_Success)
    return NULL;

  return entry;
}

SceneEditorObject *seo_list_new_entry(SceneEditorObjectList *list) {

  return (SceneEditorObject *)dyli_new_entry(
      (void *)&list->entries, &list->capacity, &list->length,
      sizeof(SceneEditorObject), "Scene Editor Object list");
}

DynamicListStatus seo_list_remove(SceneEditorObjectList *list,
                                  SceneEditorObject *entry) {

  return dyli_remove((void *)list->entries, &list->length,
                     sizeof(SceneEditorObject), (void *)entry,
                     "Scene Editor Object list");
}

DynamicListStatus seo_list_destroy(SceneEditorObjectList *list) {

  for (size_t i = 0; i < list->length; i++)
    seo_mesh_list_destroy(&list->entries[i].meshes);

  return dyli_free((void *)list->entries, &list->capacity, &list->capacity);
}

/* === SEO MESH === */

DynamicListStatus seo_mesh_list_create(SceneEditorObjectMeshList *list,
                                       size_t capacity) {
  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(SceneEditorObjectMesh), capacity,

                     "Scene Editor Object Mesh List");
}

DynamicListStatus seo_mesh_list_insert(SceneEditorObjectMeshList *list,
                                       SceneEditorObjectMesh *entry) {
  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(SceneEditorObjectMesh), (void *)entry, 1,
                     "Scene Editor Object Mesh List");
}

SceneEditorObjectMesh *
seo_mesh_list_new_entry(SceneEditorObjectMeshList *list) {
  return (SceneEditorObjectMesh *)dyli_new_entry(
      (void *)&list->entries, &list->capacity, &list->length,
      sizeof(SceneEditorObjectMesh), "Scene Editor Object Mesh List");
}

DynamicListStatus seo_mesh_list_remove(SceneEditorObjectMeshList *list,
                                       SceneEditorObjectMesh *entry) {
  return dyli_remove((void *)list->entries, &list->length,
                     sizeof(SceneEditorObjectMesh), (void *)entry,
                     "Scene Editor Object Mesh List");
}

DynamicListStatus seo_mesh_list_destroy(SceneEditorObjectMeshList *list) {
  return dyli_free((void *)list->entries, &list->capacity, &list->length);
}
