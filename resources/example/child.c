#include "child.h"
#include "primitive.h"

void example_child(Scene *scene, renderer *renderer) {

  Mesh *parent_cube;
  example_primitive(parent_cube, (vec3){3.0f, 2.0f, 1.0f}, scene, renderer);
  Mesh *child_cube_A, *child_cube_B;
  example_primitive(child_cube_A, (vec3){-4.0f, -2.0f, -1.0f}, scene, renderer);
  example_primitive(child_cube_B, (vec3){-3.0f, -9.0f, 1.0f}, scene, renderer);

  mesh_add_child(child_cube_A, parent_cube);
  mesh_add_child(child_cube_B, parent_cube);
}
