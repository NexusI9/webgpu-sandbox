#include "primitive.h"

#include <stddef.h>

#include "backend/context.h"
#include "runtime/engine/add.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/primitive/cube.h"
#include "runtime/scene/add.h"
#include "runtime/shader/core.h"
#include "runtime/systems/scene_system.h"

void example_primitive(Engine *engine, Mesh *cube, vec3 position,
                       const RenderPipeline *pipeline) {

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
