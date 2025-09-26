#include "primitive.h"

#include <stddef.h>

#include "backend/context.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/primitive/cube.h"
#include "runtime/scene/add.h"
#include "runtime/shader/core.h"

void example_primitive(Mesh *cube, vec3 position, Scene *scene,
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

  scene_add_mesh(scene, cube, NULL, SceneAddFlag_None);
}
