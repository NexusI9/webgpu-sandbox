#include "scene.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <assert.h>
#include <stdio.h>

scene scene_create(camera camera, viewport viewport) {

  scene scene;

  // set camera
  scene.camera = camera;
  scene.viewport = viewport;

  // init mesh list
  scene.mesh_list.items =
      malloc(SCENE_MESH_LIST_DEFAULT_CAPACITY * sizeof(mesh));
  scene.mesh_list.length = 0;
  scene.mesh_list.capacity = SCENE_MESH_LIST_DEFAULT_CAPACITY;

  return scene;
}

mesh *scene_add_mesh(scene *scene, mesh mesh) {

  mesh_list *mesh_list = &scene->mesh_list;

  // BUILD MESH
  // build shader (establish pipeline from previously set bind groups)
  mesh_build(&mesh);

  // ADD MESH TO LIST
  // eventually expand mesh array if overflow
  if (mesh_list->length == mesh_list->capacity) {
    mesh_list->capacity *= 2;
    mesh_list = realloc(mesh_list, mesh_list->capacity);

    perror("Scene mesh list reached full capacity"), exit(0);
    return NULL;
  } else {
    // Copy mesh to list
    mesh_list->items[mesh_list->length++] = mesh;
  }

  return &mesh_list->items[mesh_list->length - 1];
}

void scene_draw(scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(&scene->camera);

  // loop through mesh list and draw meshes
  for (int i = 0; i < scene->mesh_list.length; i++) {
    mesh_draw(&scene->mesh_list.items[i], render_pass, &scene->camera,
              &scene->viewport);
  }
}
