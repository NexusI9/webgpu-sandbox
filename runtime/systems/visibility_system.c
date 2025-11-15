#include "visibility_system.h"
#include "backend/renderer/batch.h"
#include "backend/renderer/core.h"
#include "backend/renderer/visibility.h"
#include "backend/std_pipeline/core.h"
#include "runtime/engine/core.h"
#include "runtime/gizmo/core.h"
#include "runtime/mesh/core.h"
#include "runtime/systems/scene_system.h"
#include <stdint.h>

static const MeshShader mesh_std_shaders[] = {
    MeshShader_Texture,
    MeshShader_Solid,
    MeshShader_Wireframe,
};

static const uint8_t mesh_std_shaders_length =
    sizeof(mesh_std_shaders) / sizeof(mesh_std_shaders[0]);

static inline void
visibility_system_enable_in_light_reflection(Scene *, Renderer *, Mesh *);
static inline void
visibility_system_disable_in_light_reflection(Scene *, Renderer *, Mesh *);

void visibility_system_enable_in_light_reflection(Scene *scene,
                                                  Renderer *renderer,
                                                  Mesh *mesh) {

  render_pass_enable_mesh(&scene->probes.reflection_probe.pass, mesh);
  render_pass_enable_mesh(&scene->probes.reflection_plane.pass, mesh);
  render_pass_enable_mesh(&scene->lights.point.shadow.pass, mesh);
  render_pass_enable_mesh(&scene->lights.spot.shadow.pass, mesh);
}

void visibility_system_disable_in_light_reflection(Scene *scene,
                                                   Renderer *renderer,
                                                   Mesh *mesh) {

  render_pass_disable_mesh(&scene->probes.reflection_probe.pass, mesh);
  render_pass_disable_mesh(&scene->probes.reflection_plane.pass, mesh);
  render_pass_disable_mesh(&scene->lights.point.shadow.pass, mesh);
  render_pass_disable_mesh(&scene->lights.spot.shadow.pass, mesh);
}

/**
   Show the mesh only in necessary pipelines (meaning not the selection one) and
   resync with the lights and reflection passes.

   Initially we used the function renderer_show_mesh, however the issue with
   this function is that it basically traverse all the layouts from all draw
   modes, and if it finds the mesh in the layout source list, then it enables
   it. However by doing so it also enable the mesh in unwanted layouts such as
   the Selections-related layouts.
   As a result when we hid/shown the mesh it also activated them in the
   selection, which is not the desiger result.

   To address this we had two solution: either add a "black-list"/ "exclusion
   system", to not enable the mesh in certain pipeline, however this would
   require to have a bit/ flag based pipeline system, which has some limitation
   in terms of amount (what if in the future we need 1 << 64+ pipelines).
   Or we can just take advantage of the batch renderer flexibility to instead
   target specific batch that have the pipeline we want.

   We now switch the approach: instead of simply enabling by
   "mesh pointer", we now enable by "Pipeline Type".
   Meaning in all draw modes, we only enable the mesh within the layouts' that
   own a certain pipeline.

   Such new approach ensure we only target the desired layout. To do so we
   basically retrieve each pipelines from the Mesh Standards Shaders that are
   used during the draw modes (Solid/ Texture and Wireframe) and only enable the
   mesh in each shader repsectives pipelines.
 */
void visibility_system_show_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {

  for (uint8_t i = 0; i < mesh_std_shaders_length; i++) {

    Shader *shader = mesh_shader(mesh, mesh_std_shaders[i]);

    if (shader) {
      const RenderPipelineType pipeline =
          std_render_pipeline_type(*shader->pipeline);

      renderer_show_mesh_in_pipeline(renderer, pipeline, mesh);
    }
  }

  visibility_system_enable_in_light_reflection(scene, renderer, mesh);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_hide_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {

  for (uint8_t i = 0; i < mesh_std_shaders_length; i++) {

    Shader *shader = mesh_shader(mesh, mesh_std_shaders[i]);

    if (shader) {
      const RenderPipelineType pipeline =
          std_render_pipeline_type(*shader->pipeline);

      renderer_hide_mesh_in_pipeline(renderer, pipeline, mesh);
    }
  }

  visibility_system_disable_in_light_reflection(scene, renderer, mesh);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_toggle_mesh(Scene *scene, Renderer *renderer,
                                   Mesh *mesh) {

  RendererStatus mesh_state;
  for (uint8_t i = 0; i < mesh_std_shaders_length; i++) {

    Shader *shader = mesh_shader(mesh, mesh_std_shaders[i]);

    if (shader) {
      const RenderPipelineType pipeline =
          std_render_pipeline_type(*shader->pipeline);

      mesh_state =
          renderer_visibility_toggle_mesh_in_pipeline(renderer, pipeline, mesh);
    }
  }

  if (RendererStatus_MeshVisible == mesh_state) {

    visibility_system_enable_in_light_reflection(scene, renderer, mesh);
    mesh_ref_list_remove(
        renderer_mesh_state(renderer, RendererMeshStates_Hidden), mesh);

  } else if (RendererStatus_MeshHidden == mesh_state) {

    visibility_system_disable_in_light_reflection(scene, renderer, mesh);
    mesh_ref_list_insert(
        renderer_mesh_state(renderer, RendererMeshStates_Hidden), mesh);
  }

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_show_mesh_ref_list(Scene *scene, Renderer *renderer,
                                          MeshRefList *list) {

  for (size_t i = 0; i < list->length; i++)
    visibility_system_show_mesh(scene, renderer, list->entries[i]);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_hide_mesh_ref_list(Scene *scene, Renderer *renderer,
                                          MeshRefList *list) {

  for (size_t i = 0; i < list->length; i++)
    visibility_system_hide_mesh(scene, renderer, list->entries[i]);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}
