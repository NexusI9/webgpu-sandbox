#include "show.h"

/**
   Show the mesh by pushing it to the pipeline ref list
 */
void scene_show_mesh(Scene *scene, Mesh *mesh, const ScenePipeline pipeline) {
  
  // prevent duplicate
  if (mesh_ref_list_find(&scene->pipelines[pipeline], mesh) != NULL)
    return;
  
  mesh_ref_list_insert(&scene->pipelines[pipeline], mesh);
}

/**
   Hide the mesh by removing it from the pipelines ref list.
 */
void scene_hide_mesh(Scene *scene, Mesh *mesh, const ScenePipeline pipeline) {
  mesh_ref_list_remove(&scene->pipelines[pipeline], mesh);
}

void scene_show_mesh_ref_list(Scene *scene, MeshRefList *list,
                                    const ScenePipeline pipeline) {
  for (size_t i = 0; i < list->length; i++)
    scene_show_mesh(scene, list->entries[i], pipeline);
}

void scene_hide_mesh_ref_list(Scene *scene, MeshRefList *list,
                                    const ScenePipeline pipeline) {
  for (size_t i = 0; i < list->length; i++)
    scene_hide_mesh(scene, list->entries[i], pipeline);
}
