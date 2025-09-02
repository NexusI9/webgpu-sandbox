#include "glass.h"
#include "../backend/renderer/scene/std_pipeline/modules/glass/glass.h"
#include "../runtime/mesh/shader/shader.h"
#include "webgpu/webgpu.h"

void example_glass_box(Scene *scene) {
  Mesh *mesh = scene_new_mesh(scene);

  Primitive prim = primitive_icosphere();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &prim,
                                  .name = "Glass Box Mesh",
                                  .device = scene_device(scene),
                                  .queue = scene_queue(scene),
                              });

  mesh_shader_create(mesh, &(ShaderCreateDescriptor){
                               .pipeline = std_pipeline(PipelineType_GlassBox),
                               .label = "Glass Box",
                               .name = "Glass Box",
                               .device = scene_device(scene),
                               .queue = scene_queue(scene),
                           });

  mesh_set_position(mesh, (vec3){2.0f, 4.4f, -3.0f});
  mesh_set_rotation(mesh, (vec3){180.0f, 0.0f, 0.0f});
  mesh_set_scale(mesh, (vec3){2.0f, 2.0f, 2.0f});

  scene_add_mesh(scene, mesh, NULL);

  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 0.5f, 1.0f, 1.0f},
                                 .roughness = 0.23f,
                                 .frost_scale = 700.0f,
                                 .frost_strength = 0.4f,
                             });

  shader_update_texture_view(mesh_shader(mesh, MeshShader_Texture), 1, 1,
                             scene->renderer.texture.skybox.cubemap,
                             WGPUTextureFormat_BGRA8Unorm);
}

void example_glass_probe(Scene *scene, bool debug) {

  SceneEditorObject *grid_probe =
      scene_add_probe_reflection_grid(scene, &(ProbeReflectionGridDescriptor){
                                                 .count = {3, 3, 3},
                                                 .size = {30.0f, 30.0f, 30.0f},
                                             });

  Mesh *mesh = scene_new_mesh(scene);

  Primitive prim = primitive_icosphere();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &prim,
                                  .name = "Glass Probe Plane",
                                  .device = scene_device(scene),
                                  .queue = scene_queue(scene),
                              });

  mesh_shader_create(mesh,
                     &(ShaderCreateDescriptor){
                         .pipeline = std_pipeline(PipelineType_GlassProbe),
                         .label = "Glass Probe Plane",
                         .name = "Glass Probe Plane",
                         .device = scene_device(scene),
                         .queue = scene_queue(scene),
                     });

  mesh_set_position(mesh, (vec3){0.0f, 0.2f, 0.0f});
  mesh_set_rotation(mesh, (vec3){180.0f, 0.0f, 0.0f});
  mesh_set_scale(mesh, (vec3){3.0f, 3.0f, 3.0f});

  scene_add_mesh(scene, mesh, NULL);

  // TODO: Put this in the scene build for automation ?

  // link glass settings
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                 .frost_scale = 1.0f,
                                 .frost_strength = 0.0f,
                                 .roughness = 0.145f,
                             });

  // link probe lists (position, radius)
  shader_update_uniform_buffer(
      mesh_shader(mesh, MeshShader_Texture), 1, 1,
      ssbo_buffer_handle(&scene->renderer.ssbo, SSBOType_ProbeReflection), 0,
      ShaderBufferLifetime_Release);

  // link UBO
  shader_update_uniform_buffer(mesh_shader(mesh, MeshShader_Texture), 1, 2,
                               ubo_buffer_handle(&scene->renderer.ubo), 0,
                               ShaderBufferLifetime_Release);

  // link probe color texture
  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 3,
      scene->probes_reflection.pass.color.attachment.view,
      WGPUTextureFormat_BGRA8Unorm);

  ProbeReflectionGridListDebug debug_options = {
      .scene_debug = &scene->debug,
      .max_views = 16,
  };

  probe_reflection_grid_list_draw(&scene->probes_reflection,
                                  debug ? &debug_options : NULL);
}


void example_glass_probe_planar(Scene *scene, bool debug) {

  SceneEditorObject *grid_probe =
      scene_add_probe_reflection_grid(scene, &(ProbeReflectionGridDescriptor){
                                                 .count = {3, 3, 3},
                                                 .size = {30.0f, 30.0f, 30.0f},
                                             });

  Mesh *mesh = scene_new_mesh(scene);

  Primitive prim = primitive_icosphere();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &prim,
                                  .name = "Glass Probe Plane",
                                  .device = scene_device(scene),
                                  .queue = scene_queue(scene),
                              });

  mesh_shader_create(mesh,
                     &(ShaderCreateDescriptor){
                         .pipeline = std_pipeline(PipelineType_GlassProbe),
                         .label = "Glass Probe Plane",
                         .name = "Glass Probe Plane",
                         .device = scene_device(scene),
                         .queue = scene_queue(scene),
                     });

  mesh_set_position(mesh, (vec3){0.0f, 0.2f, 0.0f});
  mesh_set_rotation(mesh, (vec3){180.0f, 0.0f, 0.0f});
  mesh_set_scale(mesh, (vec3){3.0f, 3.0f, 3.0f});

  scene_add_mesh(scene, mesh, NULL);

  // TODO: Put this in the scene build for automation ?

  // link glass settings
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                 .frost_scale = 1.0f,
                                 .frost_strength = 0.0f,
                                 .roughness = 0.145f,
                             });

  // link probe lists (position, radius)
  shader_update_uniform_buffer(
      mesh_shader(mesh, MeshShader_Texture), 1, 1,
      ssbo_buffer_handle(&scene->renderer.ssbo, SSBOType_ProbeReflection), 0,
      ShaderBufferLifetime_Release);

  // link UBO
  shader_update_uniform_buffer(mesh_shader(mesh, MeshShader_Texture), 1, 2,
                               ubo_buffer_handle(&scene->renderer.ubo), 0,
                               ShaderBufferLifetime_Release);

  // link probe color texture
  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 3,
      scene->probes_reflection.pass.color.attachment.view,
      WGPUTextureFormat_BGRA8Unorm);

  ProbeReflectionGridListDebug debug_options = {
      .scene_debug = &scene->debug,
      .max_views = 16,
  };

  probe_reflection_grid_list_draw(&scene->probes_reflection,
                                  debug ? &debug_options : NULL);
}
