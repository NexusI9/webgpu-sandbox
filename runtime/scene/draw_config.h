#ifndef _SCENE_PASS_CONFIG_H_
#define _SCENE_PASS_CONFIG_H_

#include "runtime/mesh/shader/shader.h"
#include "./core.h"
#include "debug/core.h"
#include "renderer/core.h"
#include "renderer/render_pass/core.h"
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
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Fixed_Selection),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_boundbox,
              },
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
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Fixed_Selection),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_boundbox,
              },
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
      .length = 7,
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
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Fixed_Selection),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_boundbox,
              },
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
      .length = 7,
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
                  .meshes =
                      scene_pipeline(scene, ScenePipeline_Fixed_Selection),
                  .shader = MeshShader_Wireframe,
                  .topology_callback = mesh_topology_boundbox,
              },
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

  RenderPassList *pass_list = scene->renderer.draw.pass;

  for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++) {

    RenderPassListCreate list_config = {
        .device = scene_renderer_device(&scene->renderer),
        .queue = scene_renderer_queue(&scene->renderer),
        .swapchain = scene_renderer_swapchain(&scene->renderer),
        .multisample = multisample,
        .width = scene_renderer_width(&scene->renderer),
        .height = scene_renderer_height(&scene->renderer),
    };

    render_pass_list_create(&pass_list[i], &list_config);

    RenderPassColorAttachment scene_color_attachment = {
        .view = pass_list->resolve_view,
        .clear_value = scene->renderer.background,
        .load_op = WGPULoadOp_Clear,
        .store_op = WGPUStoreOp_Store,
        .depth_slice = WGPU_DEPTH_SLICE_UNDEFINED,
    };

    RenderPassDepthAttachment scene_depth_attachment = {
        .view = NULL,
        // Allow depth write
        .read_only = false,
        // Far plane
        .clear_value = 1.0f,
        // Keep depth for later use
        .store_op = WGPUStoreOp_Store,
        // Clear depth at start of render pass
        .load_op = WGPULoadOp_Clear,
    };

    // add scene draw list
    const RenderPassListInsert scene_pass = {
        .label = "Scene Render Pass",
        .multisample = multisample,
        .width = scene_renderer_width(&scene->renderer),
        .height = scene_renderer_height(&scene->renderer),
        .color = &scene_color_attachment,
        .depth = &scene_depth_attachment,
        .draw_list = scene_draw_list[i],
    };

    render_pass_list_insert_pass(&pass_list[i], &scene_pass);

    // add gizmo draw list

    RenderPassColorAttachment gizmo_color_attachment = {
        .view = pass_list->resolve_view,
        .clear_value = 0,
        .load_op = WGPULoadOp_Load,
        .store_op = WGPUStoreOp_Store,
        .depth_slice = WGPU_DEPTH_SLICE_UNDEFINED,
    };

    RenderPassDepthAttachment gizmo_depth_attachment = {
        .view = NULL,
        .read_only = false,
        .clear_value = 1.0f,
        // clear previously rendered depth
        .load_op = WGPULoadOp_Clear,
        // do not store it afterward
        .store_op = WGPUStoreOp_Discard,
    };

    const RenderPassListInsert gizmo_pass = {
        .label = "Gizmo Render Pass",
        .multisample = multisample,
        .width = scene_renderer_width(&scene->renderer),
        .height = scene_renderer_height(&scene->renderer),
        .color = &gizmo_color_attachment,
        .depth = &gizmo_depth_attachment,
        .draw_list = &gizmo_draw_list,
    };

    render_pass_list_insert_pass(&pass_list[i], &gizmo_pass);
  }
}

#endif
