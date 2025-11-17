#include "primitive.h"

void example_primitive(Engine *engine, Mesh *cube, vec3 position,
                       RenderPipeline *const *pipeline) {

  Primitive cube_prim = primitive_cube();
  // cube = scene_new_mesh(scene);

  mesh_create_primitive(cube, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &cube_prim,
                                  .name = "cube",
                              });

  mesh_shader_create(cube, &(ShaderCreateDescriptor){
                               .pipeline = pipeline,
                               .name = "cube",
                           });

  mesh_set_position(cube, position);

  engine_scene_add_mesh(engine, cube, NULL, EngineAddFlag_None);
}
