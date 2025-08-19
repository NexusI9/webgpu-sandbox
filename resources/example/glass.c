#include "glass.h"
#include "../backend/renderer/scene/std_pipeline/layouts/layout.glass.h"

void example_glass(Scene *scene) {
  Mesh *cube = scene_new_mesh(scene);

  Primitive prim = primitive_icosphere();

  mesh_create_primitive(cube, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &prim,
                                  .name = "cube",
                                  .device = scene_device(scene),
                                  .queue = scene_queue(scene),
                              });

  mesh_shader_create(cube, &(ShaderCreateDescriptor){
                               .pipeline = std_pipeline(PipelineType_Glass),
                               .label = "cube",
                               .name = "cube",
                               .device = scene_device(scene),
                               .queue = scene_queue(scene),
                           });

  mesh_translate(cube, (vec3){2.0f, 4.4f, -3.0f});
  mesh_rotate(cube, (vec3){180.0f, 0.0f, 0.0f});
  mesh_scale(cube, (vec3){2.0f, 2.0f, 2.0f});

  scene_add_mesh(scene, cube, NULL);

  shader_update_uniform(mesh_shader_texture(cube), 0, 3,
                        &(GlassUniform){
                            .color = {1.0f, 0.5f, 1.0f, 1.0f},
                            .roughness = 0.23f,
                            .frost_scale = 700.0f,
                            .frost_strength = 0.4f,
                        });

  shader_update_texture_view(mesh_shader_texture(cube), 1, 0,
                             scene->renderer.texture.skybox.cubemap,
                             WGPUTextureFormat_BGRA8Unorm);
}
