#include "scene.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <assert.h>
#include <stdio.h>

static mesh *scene_add_mesh(mesh_list *, mesh *);
static void scene_draw_mesh_list(scene *, WGPURenderPassEncoder *, mesh_list *);
static void scene_init_mesh_list(mesh_list *);

scene scene_create(camera camera, viewport viewport) {

  scene scene;

  // set camera
  scene.camera = camera;
  scene.viewport = viewport;

  // init mesh lists
  scene_init_mesh_list(&scene.meshes.solid);
  scene_init_mesh_list(&scene.meshes.alpha);

  return scene;
}

mesh *scene_add_mesh_solid(scene *scene, mesh *mesh) {

  return scene_add_mesh(&scene->meshes.solid, mesh);
}

mesh *scene_add_mesh_alpha(scene *scene, mesh *mesh) {

  return scene_add_mesh(&scene->meshes.alpha, mesh);
}

void scene_draw(scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(&scene->camera);
  // draw solid meshes first
  scene_draw_mesh_list(scene, render_pass, &scene->meshes.solid);
  // draw transparent meshes then
  scene_draw_mesh_list(scene, render_pass, &scene->meshes.alpha);
}

mesh *scene_add_mesh(mesh_list *mesh_list, mesh *mesh) {

  // BUILD MESH
  // build shader (establish pipeline from previously set bind groups)
  mesh_build(mesh);

  // ADD MESH TO LIST
  // eventually expand mesh array if overflow

  if (mesh_list->length == mesh_list->capacity) {
    mesh_list->capacity *= 2;
    mesh_list = realloc(mesh_list, mesh_list->capacity);

    perror("Scene mesh list reached full capacity"), exit(0);
    return NULL;
  } else {
    // Copy mesh to list
    mesh_list->items[mesh_list->length++] = *mesh;
  }

  return &mesh_list->items[mesh_list->length - 1];
}

void scene_draw_mesh_list(scene *scene, WGPURenderPassEncoder *render_pass,
                          mesh_list *mesh_list) {

  // loop through mesh list and draw meshes
  for (int i = 0; i < mesh_list->length; i++) {
    mesh_draw(&mesh_list->items[i], render_pass, &scene->camera,
              &scene->viewport);
  }
}

void scene_init_mesh_list(mesh_list *mesh_list) {

  mesh_list->items = malloc(SCENE_MESH_LIST_DEFAULT_CAPACITY * sizeof(mesh));
  mesh_list->length = 0;
  mesh_list->capacity = SCENE_MESH_LIST_DEFAULT_CAPACITY;
}
