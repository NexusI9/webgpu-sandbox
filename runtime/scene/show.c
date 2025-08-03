#include "show.h"
#include "core.h"

/**
   Show the mesh by pushing it to the pipeline ref list
 */
SceneStatus scene_show_mesh(Scene *scene, Mesh *mesh,
                            const ScenePipeline pipeline) {

  // prevent duplicate
  if (mesh_ref_list_find(&scene->pipelines[pipeline], mesh) != NULL)
    return SceneStatus_MeshAlreadyExists;

  printf("show mesh\n");
  Mesh *insert = mesh_ref_list_insert(&scene->pipelines[pipeline], mesh);

  printf("insert mesh: %p\n", insert);
  if (insert == NULL)
    return SceneStatus_MeshInsertFail;

  return SceneStatus_Success;
}

/**
   Hide the mesh by removing it from the pipelines ref list.
 */
SceneStatus scene_hide_mesh(Scene *scene, Mesh *mesh,
                            const ScenePipeline pipeline) {
  DynamicListStatus remove =
      mesh_ref_list_remove(&scene->pipelines[pipeline], mesh);

  printf("[%p] hide: %d\n", mesh, remove);
  return SceneStatus_Success;
}

SceneStatus scene_show_mesh_ref_list(Scene *scene, MeshRefList *list,
                                     const ScenePipeline pipeline) {

  SceneStatus status = SceneStatus_Success;

  for (size_t i = 0; i < list->length; i++) {
    SceneStatus show = scene_show_mesh(scene, list->entries[i], pipeline);
    if (show != SceneStatus_Success)
      status = show;
  }

  return status;
}

SceneStatus scene_hide_mesh_ref_list(Scene *scene, MeshRefList *list,
                                     const ScenePipeline pipeline) {
  SceneStatus status = SceneStatus_Success;

  for (size_t i = 0; i < list->length; i++) {
    SceneStatus show = scene_hide_mesh(scene, list->entries[i], pipeline);
    if (show != SceneStatus_Success)
      status = show;
  }

  return status;
}
