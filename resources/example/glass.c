#include "glass.h"
#include "../backend/std_pipeline/std_pipeline.h"

#include "../backend/std_pipeline/modules/glass_probe_grid/glass_probe_grid.h"
#include "../runtime/mesh/shader/shader.h"
#include "webgpu/webgpu.h"

void example_glass_probe_grid(Scene *scene, bool debug) {

  SceneEditorObject *grid_probe =
      scene_add_probe_reflection_grid(scene, &(ProbeReflectionGridDescriptor){
                                                 .count = {3, 3, 3},
                                                 .scale = {30.0f, 30.0f, 30.0f},
                                             });

  Mesh *mesh = scene_new_mesh(scene);

  Primitive prim = primitive_icosphere();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &prim,
                                  .name = "Glass Probe Sphere",
                                  .device = scene_device(scene),
                                  .queue = scene_queue(scene),
                              });

  mesh_shader_create(mesh,
                     &(ShaderCreateDescriptor){
                         .pipeline = std_pipeline(PipelineType_GlassProbeGrid),
                         .label = "Glass Probe Sphere",
                         .name = "Glass Probe Sphere",
                         .device = scene_device(scene),
                         .queue = scene_queue(scene),
                     });

  mesh_set_position(mesh, (vec3){0.0f, 6.0f, 0.0f});
  mesh_set_rotation(mesh, (vec3){180.0f, 0.0f, 0.0f});
  mesh_set_scale(mesh, (vec3){3.0f, 3.0f, 3.0f});

  scene_add_mesh(scene, mesh, NULL);

  // TODO: Put this in the scene build for automation ?

  // link glass settings
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                 .frost_scale = 80.0f,
                                 .frost_strength = 10.0f,
                                 .roughness = 0.145f,
                             });

  // link probe lists (position, radius)
  shader_update_uniform_buffer(
      mesh_shader(mesh, MeshShader_Texture), 1, 1,
      ssbo_buffer_handle(&scene->renderer.ssbo, SSBOType_ProbeGridReflection),
      0, ShaderBufferLifetime_Release);

  // link UBO
  shader_update_uniform_buffer(mesh_shader(mesh, MeshShader_Texture), 1, 2,
                               ubo_buffer_handle(&scene->renderer.ubo), 0,
                               ShaderBufferLifetime_Release);

  // link probe color texture
  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 3,
      scene->probes_reflection.pass.color.attachment.view,
      TEXTURE_FORMAT_OFFSCREEN_DEFAULT);

  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 5,
      scene_environment_skybox(&scene->environment)->view,
      TEXTURE_FORMAT_OFFSCREEN_DEFAULT);

  ProbeReflectionListDebug debug_options = {
      .scene_debug = &scene->debug,
      .max_views = 16,
  };

  probe_reflection_grid_list_draw(&scene->probes_reflection,
                                  debug ? &debug_options : NULL);
}

void example_glass_probe_plane(Scene *scene, bool debug) {

  const float scale = 20.0f;

  SceneEditorObject *plane_probe = scene_add_probe_reflection_plane(
      scene, &(ProbeReflectionPlaneDescriptor){
                 .far = 100.0f,
                 .near = 0.1f,
                 .normal = {0.0f, 1.0f, 0.0f},
                 .scale = {scale + 5.0f, scale + 5.0f, scale + 5.0f},
                 .distance = 3.0f,
                 .camera = scene->active_camera,
             });

  Mesh *mesh = scene_new_mesh(scene);
  Primitive prim = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &prim,
                                  .name = "Glass Probe Plane",
                                  .device = scene_device(scene),
                                  .queue = scene_queue(scene),
                              });

  mesh_shader_create(mesh,
                     &(ShaderCreateDescriptor){
                         .pipeline = std_pipeline(PipelineType_GlassProbePlane),
                         .label = "Glass Probe Plane",
                         .name = "Glass Probe Plane",
                         .device = scene_device(scene),
                         .queue = scene_queue(scene),
                     });

  mesh_set_position(mesh, (vec3){0.0f, 0.2f, 0.0f});
  mesh_set_rotation(mesh, (vec3){180.0f, 0.0f, 0.0f});
  mesh_set_scale(mesh, (vec3){scale, scale, scale});

  scene_add_mesh(scene, mesh, NULL);

  // TODO: Put this in the scene build for automation ?

  // link glass settings
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                 .frost_scale = 20.0f,
                                 .frost_strength = 2.0f,
                                 .roughness = 0.145f,
                             });

  // link probe lists (position, radius)
  shader_update_uniform_buffer(
      mesh_shader(mesh, MeshShader_Texture), 1, 1,
      ssbo_buffer_handle(&scene->renderer.ssbo, SSBOType_ProbePlaneReflection),
      0, ShaderBufferLifetime_Release);


  // link UBO
  shader_update_uniform_buffer(mesh_shader(mesh, MeshShader_Texture), 1, 2,
                               ubo_buffer_handle(&scene->renderer.ubo), 0,
                               ShaderBufferLifetime_Release);

  // link probe color texture
  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 3,
      scene->planes_reflection.pass.color.attachment.view,
      TEXTURE_FORMAT_OFFSCREEN_DEFAULT);

  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 5,
      scene_environment_skybox(&scene->environment)->view,
      TEXTURE_FORMAT_OFFSCREEN_DEFAULT);
}
