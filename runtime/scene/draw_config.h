#ifndef _SCENE_PASS_CONFIG_H_
#define _SCENE_PASS_CONFIG_H_

#include "./core.h"
#include "backend/context.h"
#include "debug/core.h"
#include "renderer/core.h"
#include "renderer/render_pass/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/shader.h"
#include "runtime/scene/renderer/render_pass/texture.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

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

  const RenderPassDrawListDescriptor
      *scene_draw_list[SCENE_RENDERER_DRAW_MODE_COUNT] = {
          [SceneRendererDrawMode_Texture] = &texture_draw_list,
          [SceneRendererDrawMode_Solid] = &solid_draw_list,
          [SceneRendererDrawMode_Wireframe] = &wireframe_draw_list,
          [SceneRendererDrawMode_Boundbox] = &boundbox_draw_list,
      };

  RenderPassList *pass_list = scene->renderer.draw.render_pass;

  const double ratio = scene->renderer.context.dpi;
  const int render_width = context_width() * ratio;
  const int render_height = context_height() * ratio;

  for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++) {

    RenderPassListCreate list_config = {
        .multisample = multisample,
        .width = render_width,
        .height = render_height,
    };

    render_pass_list_create(&pass_list[i], &list_config);
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

      render_pass_list_create_shared_texture_color(
          &pass_list[i], &shared_texture_color_config,
          RenderPassTextureStorage_Keep, NULL, &shared_color_view);

      RenderPassTextureDescriptor shared_texture_depth_config = {
          .format = TEXTURE_FORMAT_DEPTH_STENCIL,
          .height = render_height,
          .width = render_width,
          .multisample = multisample,
      };

      render_pass_list_create_shared_texture_depth(
          &pass_list[i], &shared_texture_depth_config,
          RenderPassTextureStorage_Keep, NULL, &shared_depth_view);
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
            .clearValue = scene->renderer.background,
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
        .draw_list = scene_draw_list[i],
    };

    render_pass_list_insert_pass(&pass_list[i], &scene_pass);

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

    render_pass_list_insert_pass(&pass_list[i], &outline_pass);

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

    render_pass_list_insert_pass(&pass_list[i], &gizmo_pass);
  }
}

#endif
