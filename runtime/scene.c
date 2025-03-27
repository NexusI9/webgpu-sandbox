#include "scene.h"
#include "../utils/system.h"
#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "shader.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <assert.h>
#include <stdio.h>

static void scene_init_mesh_list(MeshList *);
static void scene_init_light_list(scene *);
static mesh *scene_add_mesh(MeshList *, mesh *);
static void scene_draw_mesh_list(scene *, MeshDrawMethod,
                                 WGPURenderPassEncoder *, MeshList *);

scene scene_create(camera camera, viewport viewport) {

  scene scene;

  // set camera
  scene.camera = camera;
  scene.viewport = viewport;

  // init mesh lists
  scene_init_mesh_list(&scene.meshes.solid);
  scene_init_mesh_list(&scene.meshes.alpha);

  // init lights
  scene_init_light_list(&scene);

  return scene;
}

mesh *scene_add_mesh_solid(scene *scene, mesh *mesh) {
  return scene_add_mesh(&scene->meshes.solid, mesh);
}

mesh *scene_add_mesh_alpha(scene *scene, mesh *mesh) {
  return scene_add_mesh(&scene->meshes.alpha, mesh);
}

void scene_draw(scene *scene, MeshDrawMethod draw_method,
                WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(&scene->camera);
  // draw solid meshes first
  scene_draw_mesh_list(scene, draw_method, render_pass, &scene->meshes.solid);
  // draw transparent meshes then
  scene_draw_mesh_list(scene, draw_method, render_pass, &scene->meshes.alpha);
}

mesh *scene_add_mesh(MeshList *mesh_list, mesh *mesh) {

  // BUILD MESH
  // build shader (establish pipeline from previously set bind groups)
  mesh_build(mesh, MESH_SHADER_DEFAULT);

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

void scene_draw_mesh_list(scene *scene, MeshDrawMethod draw_method,
                          WGPURenderPassEncoder *render_pass,
                          MeshList *mesh_list) {

  // loop through mesh list and draw meshes
  for (int i = 0; i < mesh_list->length; i++) {
    mesh *current_mesh = &mesh_list->items[i];
    mesh_draw(current_mesh, draw_method, render_pass, &scene->camera,
              &scene->viewport);
  }
}

void scene_init_mesh_list(MeshList *mesh_list) {

  mesh_list->items = malloc(SCENE_MESH_LIST_DEFAULT_CAPACITY * sizeof(mesh));
  mesh_list->length = 0;
  mesh_list->capacity = SCENE_MESH_LIST_DEFAULT_CAPACITY;
}

void scene_init_light_list(scene *scene) {

  // init point light list
  scene->lights.point.capacity = LIGHT_MAX_CAPACITY;
  scene->lights.point.length = 0;

  // init directional light list
  scene->lights.directional.capacity = LIGHT_MAX_CAPACITY;
  scene->lights.directional.length = 0;

  // init ambient light list
  scene->lights.ambient.capacity = LIGHT_MAX_CAPACITY;
  scene->lights.ambient.length = 0;
}

size_t scene_add_point_light(scene *scene, PointLightDescriptor *desc) {

  PointLightList *list = &scene->lights.point;
  if (list->length == list->capacity) {
    perror("Scene point light capacity reached maximum");
    return -1;
  }

  PointLight *new_light = &list->items[list->length++];
  light_create_point(new_light, desc);

  return list->length;
}

size_t scene_add_directional_light(scene *scene,
                                   DirectionalLightDescriptor *desc) {

  DirectionalLightList *list = &scene->lights.directional;
  if (list->length == list->capacity) {
    perror("Scene directional light capacity reached maximum");
    return -1;
  }

  DirectionalLight *new_light = &list->items[list->length++];
  light_create_directional(new_light, desc);

  return list->length;
}

size_t scene_add_ambient_light(scene *scene, AmbientLightDescriptor *desc) {

  AmbientLightList *list = &scene->lights.ambient;
  if (list->length == list->capacity) {
    perror("Scene ambient light capacity reached maximum");
    return -1;
  }

  AmbientLight *new_light = &list->items[list->length++];
  light_create_ambient(new_light, desc);

  return list->length;
}
