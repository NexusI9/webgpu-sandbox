#include "primitive.h"

#include <stddef.h>

#include "../runtime/primitive/cube.h"
#include "../runtime/mesh/shader/core.h"
#include "../runtime/mesh/transform.h"
#include "../runtime/primitive/core.h"
#include "../runtime/shader/core.h"
#include "../runtime/mesh/core.h"
#include "../runtime/pipeline/core.h"
#include "../runtime/scene/add.h"

void example_primitive(Mesh *cube, vec3 position, Scene *scene,
                       const Pipeline *pipeline) {

  Primitive cube_prim = primitive_cube();
  // cube = scene_new_mesh(scene);

  mesh_create_primitive(cube, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &cube_prim,
                                  .name = "cube",
                                  .device = scene_device(scene),
                                  .queue = scene_queue(scene),
                              });

  mesh_shader_create(cube, &(ShaderCreateDescriptor){
                               .pipeline = pipeline,
                               .label = "cube",
                               .name = "cube",
                               .device = scene_device(scene),
                               .queue = scene_queue(scene),
                           });

  mesh_set_position(cube, position);

  scene_add_mesh(scene, cube, NULL);
}
