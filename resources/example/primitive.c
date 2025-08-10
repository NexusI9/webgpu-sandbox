#include "primitive.h"
#include "../runtime/primitive/cube.h"

void example_primitive(Mesh *cube, vec3 position, Scene *scene) {

  Primitive cube_prim = primitive_cube();
  cube = scene_new_mesh(scene);
  mesh_create_primitive(cube, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &cube_prim,
                                  .name = "cube",
                                  .device = scene_device(scene),
                                  .queue = scene_queue(scene),
                              });

  mesh_shader_create(cube, &(ShaderCreateDescriptor){
                               .pipeline = std_pipeline(PipelineType_Default),
                               .label = "cube",
                               .name = "cube",
                               .device = scene_device(scene),
                               .queue = scene_queue(scene),
                           });

  mesh_translate(cube, position);

  scene_add_mesh(scene, cube, NULL);
}
