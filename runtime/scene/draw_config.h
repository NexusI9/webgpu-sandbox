#ifndef _SCENE_PASS_CONFIG_H_
#define _SCENE_PASS_CONFIG_H_

#include "./core.h"
#include "backend/context.h"
#include "backend/postfx/core.h"
#include "backend/std_pipeline/render_shader/bloom/bloom.h"
#include "backend/std_pipeline/render_shader/composite/composite.h"
#include "debug/core.h"
#include "renderer/core.h"
#include "renderer/render_pass/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/shader.h"
#include "runtime/scene/renderer/render_pass/texture.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include <stdio.h>

/**
   Define the scene renderer draw configurations by providing each draw mode
   their respective topology, shader callbacks as well a mesh list to draw
   during the loop.

   Can be read like :
   For each draw mode (tex/solid/wire) draw
          L for each given render pass draw
               L the mesh list with this shader and this topology.
 */
static inline void
scene_draw_layouts_init(Scene *scene,
                        const RenderPipelineMultisampleCount multisample) {

  // Common gizmo draw list configuration
  const RenderPassDrawListDescriptor gizmo_draw_list = {
      .length = 1,
      .entries =
          {
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Fixed_Front),
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },

          },
  };

  const RenderPassDrawListDescriptor outline_draw_list = {
      .length = 1,
      .entries =
          {
              {
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Fixed_Selection),
                  .shader = MeshShader_Outline,
                  .topology_callback = mesh_topology_base,
              },

          },
  };

  const RenderPassDrawLayoutDescriptor stencil_layout = {
      .meshes = scene_pipeline(scene, ScenePipeline_Fixed_Selection),
      .shader = MeshShader_Stencil,
      .topology_callback = mesh_topology_base,
  };

  // Texture draw configuration
  const RenderPassDrawListDescriptor texture_draw_list = {
      .length = 8,
      .entries =
          {
              {
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Fixed_Background),
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },
              stencil_layout,
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Dynamic_Lit),
                  .shader = MeshShader_Texture,
                  .topology_callback = mesh_topology_base,
              },
              {
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
                  .shader = MeshShader_Texture,
                  .topology_callback = mesh_topology_base,
              },
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Dynamic_Unlit),
                  .shader = MeshShader_Texture,
                  .topology_callback = mesh_topology_base,
              },
              // Fixed
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Fixed),
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_override,
              },
              // Debug
              {
                  .meshes = &scene->debug.object_list[SceneDebugObject_Ray],
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },
              {
                  .meshes = &scene->debug.object_list[SceneDebugObject_View],
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },
          },
  };

  // Solid draw configuration
  const RenderPassDrawListDescriptor solid_draw_list = {
      .length = 7,
      .entries =
          {
              stencil_layout,
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Dynamic_Lit),
                  .shader = MeshShader_Solid,
                  .topology_callback = mesh_topology_base,
              },
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Dynamic_Unlit),
                  .shader = MeshShader_Solid,
                  .topology_callback = mesh_topology_base,
              },
              {
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
                  .shader = MeshShader_Solid,
                  .topology_callback = mesh_topology_base,
              },
              // Fixed
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Fixed),
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_override,
              },
              // Debug
              {
                  .meshes = &scene->debug.object_list[SceneDebugObject_Ray],
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },
              {
                  .meshes = &scene->debug.object_list[SceneDebugObject_View],
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },

          },

  };

  // Wireframe draw configuration
  const RenderPassDrawListDescriptor wireframe_draw_list = {
      .length = 6,
      .entries =
          {
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Dynamic_Lit),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_wireframe,
              },
              {
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_wireframe,
              },
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Dynamic_Unlit),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_wireframe,
              },
              // Fixed
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Fixed),
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_override,
              },
              // Debug
              {
                  .meshes = &scene->debug.object_list[SceneDebugObject_Ray],
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },
              {
                  .meshes = &scene->debug.object_list[SceneDebugObject_View],
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },
          },

  };

  // Boundbox draw configuration
  const RenderPassDrawListDescriptor boundbox_draw_list = {
      .length = 6,
      .entries =
          {
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Dynamic_Lit),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_boundbox,
              },
              {
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_boundbox,
              },
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Dynamic_Unlit),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_boundbox,
              },
              // Fixed
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Fixed),
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_override,
              },
              // Debug
              {
                  .meshes = &scene->debug.object_list[SceneDebugObject_Ray],
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },
              {
                  .meshes = &scene->debug.object_list[SceneDebugObject_View],
                  .shader = MeshShader_Fixed,
                  .topology_callback = mesh_topology_base,
              },
          },

  };

  /*

    Below configuration won't be used in runtime out of debug purpose.

   */

  const RenderPassDrawListDescriptor *scene_draw_list[] = {
      [SceneRendererDrawMode_Texture] = &texture_draw_list,
      [SceneRendererDrawMode_Solid] = &solid_draw_list,
      [SceneRendererDrawMode_Wireframe] = &wireframe_draw_list,
      [SceneRendererDrawMode_Boundbox] = &boundbox_draw_list,
  };

  RenderPassList *pass_list = scene->renderer.draw.render_pass;

  const double ratio = scene_renderer_dpi(&scene->renderer);
  const int render_width = scene_renderer_width(&scene->renderer) * ratio;
  const int render_height = scene_renderer_height(&scene->renderer) * ratio;

  for (uint8_t mode = 0; mode < SCENE_RENDERER_DRAW_MODE_COUNT; mode++) {

    render_pass_list_create(&pass_list[mode]);
    WGPUTextureView shared_color_view;
    WGPUTextureView shared_depth_view;

    // create shared view for scene and outline since they share the same
    // stencil
    {
      RenderPassTextureDescriptor shared_texture_color_config = {
          .format = TEXTURE_FORMAT_ONSCREEN,
          .height = render_height,
          .width = render_width,
          .multisample = multisample,
      };

      render_pass_list_texture_create_shared_color(
          &pass_list[mode], &shared_texture_color_config, NULL,
          &shared_color_view, RenderPassTextureFlag_None);

      RenderPassTextureDescriptor shared_texture_depth_config = {
          .format = TEXTURE_FORMAT_DEPTH_STENCIL,
          .height = render_height,
          .width = render_width,
          .multisample = multisample,
      };

      render_pass_list_texture_create_shared_depth(
          &pass_list[mode], &shared_texture_depth_config, NULL,
          &shared_depth_view, RenderPassTextureFlag_None);
    }

    /*
                  ▗▄▄▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖
                 ▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌▐▌
                  ▝▀▚▖▐▌   ▐▛▀▀▘▐▌ ▝▜▌▐▛▀▀▘
                 ▗▄▄▞▘▝▚▄▄▖▐▙▄▄▖▐▌  ▐▌▐▙▄▄▖

                    ▗▄▄▖  ▗▄▖  ▗▄▄▖ ▗▄▄▖
                    ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
                    ▐▛▀▘ ▐▛▀▜▌ ▝▀▚▖ ▝▀▚▖
                    ▐▌   ▐▌ ▐▌▗▄▄▞▘▗▄▄▞▘

     */

    RenderPassColorAttachment scene_color_attachment = {
        .attachment = {
            .view = shared_color_view,
            .clearValue = scene->renderer.context.background,
            .loadOp = WGPULoadOp_Clear,
            .storeOp = WGPUStoreOp_Store,
            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
        }};

    RenderPassDepthAttachment scene_depth_attachment = {
        .format = TEXTURE_FORMAT_DEPTH_STENCIL,
        .attachment = {
            .view = shared_depth_view,
            .depthReadOnly = false,
            .depthClearValue = 1.0f,
            .depthStoreOp = WGPUStoreOp_Discard,
            .depthLoadOp = WGPULoadOp_Clear,
            .stencilLoadOp = WGPULoadOp_Clear,
            .stencilStoreOp = WGPUStoreOp_Store,
            .stencilClearValue = 0,
            .stencilReadOnly = false,
        }};

    const RenderPassCreateDescriptor scene_pass = {
        .type = RenderPassType_OnScreen,
        .label = "Scene Render Pass",
        .multisample = multisample,
        .width = render_width,
        .height = render_height,
        .color = &scene_color_attachment,
        .depth = &scene_depth_attachment,
        .draw_list = scene_draw_list[1 << mode],
    };

    render_pass_list_insert_pass(&pass_list[mode], &scene_pass);

    /*
         ▗▄▄▖▗▄▄▄▖▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▄▄▄▖ ▗▄▖ ▗▖  ▗▖
        ▐▌   ▐▌   ▐▌   ▐▌   ▐▌     █    █  ▐▌ ▐▌▐▛▚▖▐▌
         ▝▀▚▖▐▛▀▀▘▐▌   ▐▛▀▀▘▐▌     █    █  ▐▌ ▐▌▐▌ ▝▜▌
        ▗▄▄▞▘▐▙▄▄▖▐▙▄▄▖▐▙▄▄▖▝▚▄▄▖  █  ▗▄█▄▖▝▚▄▞▘▐▌  ▐▌

                    ▗▄▄▖  ▗▄▖  ▗▄▄▖ ▗▄▄▖
                    ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
                    ▐▛▀▘ ▐▛▀▜▌ ▝▀▚▖ ▝▀▚▖
                    ▐▌   ▐▌ ▐▌▗▄▄▞▘▗▄▄▞▘

     */

    RenderPassColorAttachment outline_color_attachment = {
        .attachment = {
            .view = shared_color_view,
            .clearValue = 0,
            .loadOp = WGPULoadOp_Load,
            .storeOp = WGPUStoreOp_Store,
            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
        }};

    RenderPassDepthAttachment outline_depth_attachment = {
        .attachment = {
            .view = shared_depth_view,
            .depthReadOnly = false,
            .depthClearValue = 1.0f,
            .depthStoreOp = WGPUStoreOp_Discard,
            .depthLoadOp = WGPULoadOp_Clear,
            .stencilLoadOp = WGPULoadOp_Load,
            .stencilStoreOp = WGPUStoreOp_Store,
            .stencilClearValue = 0,
            .stencilReadOnly = false,
        }};

    const RenderPassCreateDescriptor outline_pass = {
        .type = RenderPassType_OnScreen,
        .label = "Outline Render Pass",
        .multisample = multisample,
        .width = render_width,
        .height = render_height,
        .color = &outline_color_attachment,
        .depth = &outline_depth_attachment,
        .draw_list = &outline_draw_list,
    };

    render_pass_list_insert_pass(&pass_list[mode], &outline_pass);

    /*
                ▗▄▄▖▗▄▄▄▖▗▄▄▄▄▖▗▖  ▗▖ ▗▄▖
               ▐▌     █     ▗▞▘▐▛▚▞▜▌▐▌ ▐▌
               ▐▌▝▜▌  █   ▗▞▘  ▐▌  ▐▌▐▌ ▐▌
               ▝▚▄▞▘▗▄█▄▖▐▙▄▄▄▖▐▌  ▐▌▝▚▄▞▘

                  ▗▄▄▖  ▗▄▖  ▗▄▄▖ ▗▄▄▖
                  ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
                  ▐▛▀▘ ▐▛▀▜▌ ▝▀▚▖ ▝▀▚▖
                  ▐▌   ▐▌ ▐▌▗▄▄▞▘▗▄▄▞▘

     */

    RenderPassColorAttachment gizmo_color_attachment = {
        .attachment = {
            .view = shared_color_view,
            .clearValue = 0,
            .loadOp = WGPULoadOp_Load,
            .storeOp = WGPUStoreOp_Store,
            .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
        }};

    RenderPassDepthAttachment gizmo_depth_attachment = {
        .attachment = {
            .view = RENDER_PASS_VIEW_UNDEFINED,
            .depthReadOnly = false,
            .depthClearValue = 1.0f,
            .depthLoadOp = WGPULoadOp_Clear,
            .depthStoreOp = WGPUStoreOp_Discard,
        }};

    const RenderPassCreateDescriptor gizmo_pass = {
        .type = RenderPassType_OnScreen,
        .label = "Gizmo Render Pass",
        .multisample = multisample,
        .width = render_width,
        .height = render_height,
        .color = &gizmo_color_attachment,
        .depth = &gizmo_depth_attachment,
        .draw_list = &gizmo_draw_list,
    };

    RenderPass *last_pass =
        render_pass_list_insert_pass(&pass_list[mode], &gizmo_pass);

    /**

       ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▄▄▄▖    ▗▄▄▄▖▗▖  ▗▖
       ▐▌ ▐▌▐▌ ▐▌▐▌     █      ▐▌    ▝▚▞▘
       ▐▛▀▘ ▐▌ ▐▌ ▝▀▚▖  █      ▐▛▀▀▘  ▐▌
       ▐▌   ▝▚▄▞▘▗▄▄▞▘  █      ▐▌   ▗▞▘▝▚▖


       We use two kinds of Post Effect pipeline depending on draw mode:

       .-------------.
       |  Boundbox   | --------.
       '-------------'         |
       .-------------.         |        .- fx ------.
       |  Wireframe  | --------+------> |    Blit   |
       '-------------'         |        '-----------'
       .-------------.         |
       |    Solid    | --------'
       '-------------'
       .-------------.                   .- fx ----.     .- fx ------.
       |   Texture   | ----------------> |  Bloom  | --> | Composite |
       '-------------'                   '---------'     '-----------'

       The composite acts as an "advanced blit" with more functionalities (tone
       mapping, exposure, vignette, bloom...)

     */

    PostFxDescriptor post_fx_desc = {
        .scene_view = last_pass->color.resolve_view,
        .width = (const TextureResolution)render_width,
        .height = (const TextureResolution)render_height,
        .compute = &scene->renderer.draw.compute_pass,
    };

    post_fx_init(&last_pass->post_fx, &post_fx_desc);

    if (SceneRendererDrawMode_Solid & (1 << mode) ||
        SceneRendererDrawMode_Wireframe & (1 << mode) ||
        SceneRendererDrawMode_Boundbox & (1 << mode)) {

      post_fx_toggle_effect(&last_pass->post_fx, PostFxType_Blit);
      post_fx_update_effect_view(&last_pass->post_fx, PostFxType_Blit,
                                 PostFxViewIndex_Scene,
                                 last_pass->color.resolve_view);

    } else {

      {
        const PostFxEffectUniform bloom = {
            .bloom =
                {
                    .blur = 2,
                    .knee = 0.3f,
                    .threshold = 0.3f,
                    .downscale = 2,
                },
        };

        post_fx_toggle_effect(&last_pass->post_fx, PostFxType_Bloom);
        post_fx_update_effect_uniform(&last_pass->post_fx, PostFxType_Bloom,
                                      bloom);
      }

      {
        const PostFxEffectUniform composite = {
            .composite =
                {
                    .bloom_intensity = 1.280f,
                    .exposure = 1.0f,
                    .gamma = 1.0f,
                    .vignette_feather = 0.420f,
                    .vignette_strength = 0.720f,
                },
        };

        post_fx_toggle_effect(&last_pass->post_fx, PostFxType_Composite);
        post_fx_update_effect_uniform(&last_pass->post_fx, PostFxType_Composite,
                                      composite);
      }
    }
  }
}

#endif
