#include "show.h"

#include "core.h"
#include "renderer/render_pass/core.h"
#include "renderer/core.h"
#include "runtime/mesh/core.h"

/**
   Show the mesh by pushing it to the pipeline ref list
 */
SceneStatus scene_show_mesh(Scene *scene, Mesh *mesh,
                            const ScenePipeline pipeline) {

  render_pass_list_draw_list_enable_mesh(
      &scene->renderer.draw.render_pass[scene->renderer.draw.mode], mesh,
      scene_pipeline(scene, pipeline));

  return SceneStatus_Success;
}

/**
   Hide the mesh by removing it from the pipelines ref list.
 */
SceneStatus scene_hide_mesh(Scene *scene, Mesh *mesh,
                            const ScenePipeline pipeline) {

  render_pass_list_draw_list_disable_mesh(
      &scene->renderer.draw.render_pass[scene->renderer.draw.mode], mesh,
      scene_pipeline(scene, pipeline));

  return SceneStatus_Success;
}

SceneStatus scene_show_mesh_ref_list(Scene *scene, MeshRefList *list,
                                     const ScenePipeline pipeline) {

  render_pass_list_draw_list_enable_mesh_ref_list(
      &scene->renderer.draw.render_pass[scene->renderer.draw.mode], list,
      scene_pipeline(scene, pipeline));

  return SceneStatus_Success;
}

SceneStatus scene_hide_mesh_ref_list(Scene *scene, MeshRefList *list,
                                     const ScenePipeline pipeline) {

  render_pass_list_draw_list_enable_mesh_ref_list(
      &scene->renderer.draw.render_pass[scene->renderer.draw.mode], list,
      scene_pipeline(scene, pipeline));

  return SceneStatus_Success;
}
