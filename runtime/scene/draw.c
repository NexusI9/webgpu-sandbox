#include "draw.h"

#include "backend/postfx/core.h"
#include "core.h"
#include "runtime/light/shadow_map/draw.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/build.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "runtime/scene/renderer/render_pass/texture.h"
#include "runtime/texture/core.h"
#include "runtime/texture/create.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

static const ScenePipeline scene_dynamic_pipelines[4] = {
    ScenePipeline_Dynamic_Lit,
    ScenePipeline_Dynamic_LitShadow,
    ScenePipeline_Dynamic_Lit,
    ScenePipeline_Dynamic_Unlit,
};

void scene_set_draw_mode(Scene *scene, const SceneRendererDrawMode mode) {

  if (mode == scene->renderer.draw.mode)
    return;

  // build dynamic meshes
  for (uint8_t i = 0; i < 4; i++)
    scene_build_mesh_ref_list(scene,
                              scene_pipeline(scene, scene_dynamic_pipelines[i]),
                              scene_dynamic_pipelines[i], mode);

  // update light / reflections
  if (mode == SceneRendererDrawMode_Texture) {

    shadow_map_draw_all(
        &(ShadowMapDrawAllDescriptor){
            .mesh_list = scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
            .lights = &scene->lights,
        },
        SCENE_DEBUG_UNDEFINED);
  }

  // update renderer drawn render pass configuration
  scene_renderer_set_draw_mode(&scene->renderer, mode);
}

/**
   Recreate scene render pass list textures based on the given dimensions and
   multisample count. Since Scene render pass list texture is a mix of shared
   texture we manually pick and update them.

   Function primarily used in the UI when we adjust the scene width and height.
 */
void scene_update_render_pass_texture(
    Scene *scene, int width, int height,
    const RenderPipelineMultisampleCount multisample, const double dpi) {

  int real_width = (int)(width * dpi);
  int real_height = (int)(height * dpi);

  // Update textures
  for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++) {

    RenderPassList *pass_list = &scene->renderer.draw.render_pass[i];

    // === Color ===
    {
      // destroy previous and create new texture
      WGPUTextureView shared_color_view;

      RenderPassTextureDescriptor color_config = {
          .format = TEXTURE_FORMAT_ONSCREEN,
          .height = real_height,
          .width = real_width,
          .multisample = multisample,
      };

      render_pass_list_texture_create_shared_color(
          pass_list, &color_config, NULL, &shared_color_view,
          RenderPassTextureFlag_ReleasePrevious);


      // replace each passes color views with resized one
      for (ScenePass j = 0; j < SCENE_RENDER_PASS_COUNT; j++) {
        RenderPass *pass = &pass_list->passes[j];
        WGPUTextureView previous_resolve = pass->color.resolve_view;
        RenderPipelineMultisampleCount previous_multisample = pass->multisample;

        /*
          If previously monosample it means the resolve texture/view corresponds
          to the list shared view/ texture.

          However we already release the shared view/texutre in the
          create_shared_color function, so we set the child pass resolve
          texture/view to NULL manually.

          Monosample:

                List                      Child Passes

           .-----------.               .-- pass 1 -------.
           |   Share   |        .----> | Resolve Texture |
           |-----------|       |       |-----------------|     .-------------.
           |  Texture  |------+    .-> | Resolve View    | --> | Attachment  |
           |-----------|      |   |    '-----------------'     '-------------'
           |  View     |------|---+
           '-----------'      |   |    .---pass 2--------.
                              '------> | Resolve Texture |
                                  |    |-----------------|     .-------------.
                                  '--> | Resolve View    | --> | Attachment  |
                                       '-----------------'     '-------------'

         */
        if (previous_multisample == PipelineMultisampleCount_1x) {
          pass->color.resolve_texture = NULL;
          pass->color.resolve_view = NULL;
        }

        pass->multisample = multisample;
        pass->color.attachment.view = shared_color_view;

        // Update resolve view for multisample passes
        if (PipelineMultisampleCount_4x == pass->multisample)
          render_pass_texture_create_monosample(
              &pass->color.resolve_texture, &pass->color.resolve_view,
              &color_config, RenderPassTextureFlag_ReleasePrevious);

        if (PipelineMultisampleCount_1x == pass->multisample) {
          pass->color.resolve_view = shared_color_view;
          pass->color.resolve_texture = NULL;
          pass->color.attachment.resolveTarget = NULL;
        }

        // update post fx bingroup with the newest view
        post_fx_update_bindgroup_view(&pass->post_fx, PostFxType_Blit,
                                      previous_resolve,
                                      pass->color.resolve_view);
      }
    }

    // === Depth ===
    {
      // destroy previous and create new texture
      WGPUTextureView shared_depth_view;

      render_pass_list_texture_create_shared_depth(
          pass_list,
          &(RenderPassTextureDescriptor){
              .format = TEXTURE_FORMAT_DEPTH_STENCIL,
              .height = real_height,
              .width = real_width,
              .multisample = multisample,

          },
          NULL, &shared_depth_view, RenderPassTextureFlag_ReleasePrevious);

      // replace each passes color views with resized one
      for (ScenePass j = 0; j < SCENE_RENDER_PASS_COUNT - 1; j++)
        pass_list->passes[j].depth.attachment.view = shared_depth_view;

      // create individual depth texture for gizmo pass
      render_pass_texture_create_depth(&pass_list->passes[ScenePass_Gizmo],
                                       &(RenderPassTextureDescriptor){
                                           .format = TEXTURE_FORMAT_DEPTH,
                                           .height = real_height,
                                           .width = real_width,
                                           .multisample = multisample,
                                       },
                                       RenderPassTextureFlag_ReleasePrevious);
    }
  }
}
