#include "highlight.h"

#include <stddef.h>

#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/core.h"
#include "runtime/scene/renderer/render_pass/core.h"

void scene_selection_mesh_highlight(MeshRefList *list, void *data) {

  Scene *scene = (Scene *)data;

  MeshRefList *ref_list = scene_pipeline(scene, ScenePipeline_Fixed_Selection);

  render_pass_list_draw_list_disable_mesh_ref_list(
      &scene->renderer.draw.render_pass[scene->renderer.draw.mode], ref_list, ref_list);

  render_pass_list_draw_list_enable_mesh_ref_list(
      &scene->renderer.draw.render_pass[scene->renderer.draw.mode], list, ref_list);

  // mesh_ref_list_empty(scene_pipeline(scene, ScenePipeline_Fixed_Selection));
  // mesh_ref_list_append(
  //     list, scene_pipeline(scene, ScenePipeline_Fixed_Selection), NULL);
};

void scene_selection_seo_highlight(MeshRefList *list, void *data) {}
