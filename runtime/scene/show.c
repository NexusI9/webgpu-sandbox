#include "show.h"

#include "core.h"
#include "renderer/core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/renderer/render_pass/visibility.h"

/**
   Show the mesh by pushing it to the pipeline ref list
 */
SceneStatus scene_show_mesh(Scene *scene, Mesh *mesh) {

  render_pass_list_enable_mesh(
      scene_renderer_active_pass_list(&scene->renderer), mesh);

  return SceneStatus_Success;
}

/**
   Hide the mesh by removing it from the pipelines ref list.
 */
SceneStatus scene_hide_mesh(Scene *scene, Mesh *mesh) {

  render_pass_list_disable_mesh(
      scene_renderer_active_pass_list(&scene->renderer), mesh);

  return SceneStatus_Success;
}

SceneStatus scene_show_mesh_ref_list(Scene *scene, MeshRefList *list) {

  render_pass_list_enable_mesh_ref_list(
      scene_renderer_active_pass_list(&scene->renderer), list);

  return SceneStatus_Success;
}

SceneStatus scene_hide_mesh_ref_list(Scene *scene, MeshRefList *list) {

  render_pass_list_enable_mesh_ref_list(
      scene_renderer_active_pass_list(&scene->renderer), list);

  return SceneStatus_Success;
}
