#include "highlight.h"

void scene_selection_mesh_highlight(MeshRefList *list, void *data) {

  Scene *scene = (Scene *)data;
  mesh_ref_list_empty(scene_pipeline(scene, ScenePipeline_Fixed_Selection));
  mesh_ref_list_transfert(
      list, scene_pipeline(scene, ScenePipeline_Fixed_Selection), NULL);
};

void scene_selection_seo_highlight(MeshRefList *list, void *data) {}
