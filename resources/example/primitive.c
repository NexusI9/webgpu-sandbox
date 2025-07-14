#include "primitive.h"

void example_primitive(Mesh *cube, vec3 position, Scene *scene) {

  Primitive cube_prim = primitive_cube();
  cube = scene_new_mesh_unlit(scene);
  mesh_create_primitive(cube, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = cube_prim,
                                  .name = "cube",
                                  .device = scene->device,
                                  .queue = scene->queue,
                              });

  mesh_set_shader(cube,
                  &(ShaderCreateDescriptor){
                      .path = "./runtime/assets/shader/shader.default.wgsl",
                      .label = "cube",
                      .name = "cube",
                      .device = scene->device,
                      .queue = scene->queue,
                  });

  mesh_translate(cube, position);
}
