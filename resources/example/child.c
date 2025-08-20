#include "child.h"
#include "primitive.h"

void example_child(Scene *scene) {

  const Pipeline *pipeline = std_pipeline(PipelineType_Default);
  Mesh *parent_cube;
  example_primitive(parent_cube, (vec3){3.0f, 2.0f, 1.0f}, scene, pipeline);
  Mesh *child_cube_A, *child_cube_B;
  example_primitive(child_cube_A, (vec3){-4.0f, -2.0f, -1.0f}, scene, pipeline);
  example_primitive(child_cube_B, (vec3){-3.0f, -9.0f, 1.0f}, scene, pipeline);

  mesh_child_add(parent_cube, child_cube_A);
  mesh_child_add(parent_cube, child_cube_B);
}
