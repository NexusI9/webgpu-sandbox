#include "scene_system.h"
#include "backend/renderer/render_pass/texture.h"
#include "backend/renderer/shadow_map/draw.h"

void scene_system_set_draw_mode(Scene *scene, Renderer *renderer,
                                const RendererDrawMode mode) {

  if (mode == renderer->draw_mode)
    return;

  profiler_latency_clear_all(&renderer->profiler);

  // update light / reflections
  if (mode == RendererDrawMode_Texture) {

    renderer_draw_shadow_map_all(
        &(ShadowMapDrawAllDescriptor){
            .mesh_list =
                renderer_pipeline(renderer, RendererPipeline_Dynamic_LitShadow),
            .lights = &scene->lights,
            .profiler = &renderer->profiler},
        SCENE_DEBUG_UNDEFINED);
  }

  // update renderer drawn render pass configuration
  renderer_set_draw_mode(renderer, mode);
}

/**
   Recreate scene render pass list textures based on the given dimensions and
   multisample count. Since Scene render pass list texture is a mix of shared
   texture we manually pick and update them.

   Function primarily used in the UI when we adjust the scene width and height.
 */
void scene_system_update_render_pass_texture(Scene *scene,
    Renderer *rd, int width, int height,
    const RenderPipelineMultisampleCount multisample, const double dpi) {

  int real_width = (int)(width * dpi);
  int real_height = (int)(height * dpi);

  // Update textures
  for (uint8_t mode = 0; mode < RENDERER_DRAW_MODE_COUNT; mode++) {

    RenderPassList *pass_list = &rd->mesh_pass[mode];

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
      for (int j = 0; j < RENDERER_MESH_PASS_COUNT; j++) {
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

               List                       Child Passes

           .-----------.               .-- pass 1 -------.
           |   Share   |       .-----> | Resolve Texture |
           |-----------|      |        |-----------------|     .-------------.
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

        if (j == RENDERER_MESH_PASS_COUNT - 1) {

          post_fx_update_scene_view(&pass->post_fx, pass->color.resolve_view);
          if (RendererDrawMode_Texture & (1 << mode))
            // recreate the bloom independent texture with the new resolution
            post_fx_bloom_update_texture_resolution(&pass->post_fx, real_width,
                                                    real_height);
        }
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
      for (int j = 0; j < RENDERER_MESH_PASS_COUNT - 1; j++)
        pass_list->passes[j].depth.attachment.view = shared_depth_view;

      // create individual depth texture for gizmo pass
      render_pass_texture_create_depth(
          &pass_list->passes[RendererMeshPass_Gizmo],
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
