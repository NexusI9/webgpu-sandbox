#include "scene.h"

scene scene_create(camera cam) {

  scene s;

  // set camera
  s.camera = cam;

  // init mesh list
  s.mesh_list.items = malloc(SCENE_MESH_LIST_DEFAULT_CAPACITY * sizeof(mesh *));
  s.mesh_list.length = 0;
  s.mesh_list.capacity = SCENE_MESH_LIST_DEFAULT_CAPACITY;

  return s;
}

void scene_add_mesh(scene *scene, mesh *mesh) {

  // eventually expand mesh array if overflow
}
