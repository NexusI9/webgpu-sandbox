#include "scene.h"
#include <assert.h>

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

    mesh_list* mesh_list = &scene->mesh_list;

  // eventually expand mesh array if overflow
  if (mesh_list->length == mesh_list->capacity - 1) {
    //TODO: find a way to realloc double size (realloc doesn't seem to be included in emscripten)
      assert("Scene mesh list reached full capacity");
      return;
  }else{
      mesh_list->items[mesh_list->length++] = mesh;
  }
  
}
