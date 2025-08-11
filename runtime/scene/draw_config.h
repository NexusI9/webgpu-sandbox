#ifndef _SCENE_PASS_CONFIG_H_
#define _SCENE_PASS_CONFIG_H_

#include "./core.h"

/**
   Define the scene renderer draw configurations by providing each draw mode
   their respective topology, shader callbacks as well a mesh list to draw
   during the loop.

   Can be read like :
   For each draw mode (tex/solid/wire) draw
          L for each given render pass draw
               L the mesh list with this shader and this topology.
 */
static inline void scene_init_draw_layouts(Scene *scene) {

  const RenderPassDrawList gizmo_pass = {
      .pass = RenderPassType_Gizmo,
      .length = 1,
      .entries =
          {
              {
                  .meshes = scene_pipeline(scene, ScenePipeline_Fixed_Front),
                  .shader_callback = mesh_shader_fixed,
                  .topology_callback = mesh_topology_base,
              },

          },
  };

  // Texture draw configuration
  const RenderPassLayout texture_layout = {
      .length = 2,
      .entries =
          {
              {
                  .pass = RenderPassType_Scene,
                  .length = 6,
                  .entries =
                      {
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Fixed_Background),
                              .shader_callback = mesh_shader_fixed,
                              .topology_callback = mesh_topology_base,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_Lit),
                              .shader_callback = mesh_shader_texture,
                              .topology_callback = mesh_topology_base,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_LitShadow),
                              .shader_callback = mesh_shader_texture,
                              .topology_callback = mesh_topology_base,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_Unlit),
                              .shader_callback = mesh_shader_texture,
                              .topology_callback = mesh_topology_base,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Fixed_Selection),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_boundbox,
                          },
                          {
                              .meshes =
                                  scene_pipeline(scene, ScenePipeline_Fixed),
                              .shader_callback = mesh_shader_fixed,
                              .topology_callback = mesh_topology_override,
                          },
                      },
              },
              gizmo_pass,
          },
  };
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Texture, &texture_layout);

  // Solid draw configuration
  const RenderPassLayout solid_layout = (RenderPassLayout){
      .length = 2,
      .entries =
          {
              {
                  .pass = RenderPassType_Scene,
                  .length = 5,
                  .entries =
                      {
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_Lit),
                              .shader_callback = mesh_shader_solid,
                              .topology_callback = mesh_topology_base,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_Unlit),
                              .shader_callback = mesh_shader_solid,
                              .topology_callback = mesh_topology_base,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_LitShadow),
                              .shader_callback = mesh_shader_solid,
                              .topology_callback = mesh_topology_base,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Fixed_Selection),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_boundbox,
                          },
                          {
                              .meshes =
                                  scene_pipeline(scene, ScenePipeline_Fixed),
                              .shader_callback = mesh_shader_fixed,
                              .topology_callback = mesh_topology_override,
                          },

                      },
              },
              gizmo_pass,
          },
  };

  scene_renderer_set_draw_layout(&scene->renderer, SceneRendererDrawMode_Solid,
                                 &solid_layout);

  // Wireframe draw configuration
  const RenderPassLayout wireframe_layout = {
      .length = 2,
      .entries =
          {
              {
                  .pass = RenderPassType_Scene,
                  .length = 5,
                  .entries =
                      {
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_Lit),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_wireframe,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_LitShadow),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_wireframe,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_Unlit),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_wireframe,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Fixed_Selection),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_boundbox,
                          },
                          {
                              .meshes =
                                  scene_pipeline(scene, ScenePipeline_Fixed),
                              .shader_callback = mesh_shader_fixed,
                              .topology_callback = mesh_topology_override,
                          },
                      },
              },
              gizmo_pass,
          },
  };
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Wireframe, &wireframe_layout);

  // Boundbox draw configuration
  const RenderPassLayout boundbox_layout = {
      .length = 2,
      .entries =
          {
              {
                  .pass = RenderPassType_Scene,
                  .length = 5,
                  .entries =
                      {
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_Lit),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_boundbox,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_LitShadow),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_boundbox,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Dynamic_Unlit),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_boundbox,
                          },
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Fixed_Selection),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_boundbox,
                          },
                          {
                              .meshes =
                                  scene_pipeline(scene, ScenePipeline_Fixed),
                              .shader_callback = mesh_shader_fixed,
                              .topology_callback = mesh_topology_override,
                          },
                      },

              },
              gizmo_pass,
          },
  };

  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Boundbox, &boundbox_layout);

  /*


    Below configuration won't be used in runtime out of debug purpose.


   */

  // Fixed draw configuration (use override topology & shader)
  const RenderPassLayout fixed_layout = {
      .length = 1,
      .entries =
          {

              {
                  .pass = RenderPassType_Scene,
                  .length = 1,
                  .entries =
                      {
                          {
                              .meshes =
                                  scene_pipeline(scene, ScenePipeline_Fixed),
                              .shader_callback = mesh_shader_fixed,
                              .topology_callback = mesh_topology_override,
                          },
                      },
              },
          },
  };
  scene_renderer_set_draw_layout(&scene->renderer, SceneRendererDrawMode_Fixed,
                                 &fixed_layout);

  // Selection draw configuration
  const RenderPassLayout selection_layout = {
      .length = 1,
      .entries =
          {
              {
                  .pass = RenderPassType_Scene,
                  .length = 1,
                  .entries =
                      {
                          {
                              .meshes = scene_pipeline(
                                  scene, ScenePipeline_Fixed_Selection),
                              .shader_callback = mesh_shader_wireframe,
                              .topology_callback = mesh_topology_boundbox,
                          },
                      },

              },
          },
  };
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Selection, &selection_layout);
}

#endif
