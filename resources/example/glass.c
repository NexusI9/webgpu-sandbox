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

  shader_update_uniform(mesh_shader(mesh, MeshShader_Texture), 0, 3,
                        &(GlassUniform){
                            .color = {1.0f, 0.5f, 1.0f, 1.0f},
                            .roughness = 0.23f,
                            .frost_scale = 700.0f,
                            .frost_strength = 0.4f,
                        });

  shader_update_texture_view(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             scene->renderer.texture.skybox.cubemap,
                             WGPUTextureFormat_BGRA8Unorm);
}

void example_glass_probe(Scene *scene) {

  SceneEditorObject *grid_probe =
      scene_add_probe_reflection_grid(scene, &(ProbeReflectionGridDescriptor){
                                                 .count = {1, 1, 1},
                                                 .size = {3.0f, 3.0f, 3.0f},
                                             });

  Mesh *mesh = scene_new_mesh(scene);

  Primitive prim = primitive_cube();

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
  mesh_set_scale(mesh, (vec3){12.0f, 12.0f, 12.0f});

  scene_add_mesh(scene, mesh, NULL);

  shader_update_uniform(mesh_shader(mesh, MeshShader_Texture), 0, 3,
                        &(GlassUniform){
                            .color = {1.0f, 1.0f, 1.0f, 1.0f},
                            .frost_scale = 1.0f,
                            .frost_strength = 0.0f,
                            .roughness = 0.145f,
                        });

  // first update each meshes draw list probe list uniform
  ProbeReflectionListUniform list_uniform;
  probe_reflection_grid_list_uniform(&list_uniform, &scene->probes_reflection);

  shader_update_uniform(mesh_shader(mesh, MeshShader_Texture), 0, 4,
                        &list_uniform);

  // swap fallback view with probe render pass view
  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 0,
      scene->probes_reflection.pass.color.attachment.view,
      WGPUTextureFormat_BGRA8Unorm);

  probe_reflection_grid_list_draw(&scene->probes_reflection);
}
