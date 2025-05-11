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

static void scene_init_mesh_layer(MeshIndexedList *);
static void scene_init_light_list(scene *);
static mesh *scene_new_mesh(scene *);
static mesh *scene_layer_add_mesh(MeshIndexedList *, mesh *);
static void scene_draw_mesh_list(scene *, MeshDrawMethod,
                                 WGPURenderPassEncoder *, MeshIndexedList *);
static void scene_build_mesh_list(scene *, MeshDrawMethod, MeshIndexedList *);

scene scene_create(camera camera, viewport viewport) {

  scene scene;

  // set camera
  scene.camera = camera;
  scene.viewport = viewport;

  // init global mesh list
  scene.meshes.entries = malloc(SCENE_MESH_MAX_MESH_CAPACITY * sizeof(mesh));
  scene.meshes.length = 0;
  scene.meshes.capacity = SCENE_MESH_MAX_MESH_CAPACITY;

  // init mesh layers
  scene_init_mesh_layer(&scene.layer.lit);
  scene_init_mesh_layer(&scene.layer.unlit);

  // init lights
  scene_init_light_list(&scene);

  return scene;
}

/**
 Return the new mesh pointer from the global array and push the new pointer to
 the right scene layer
 */
mesh *scene_new_mesh_lit(scene *scene) {
  mesh *new_mesh = scene_new_mesh(scene);
  return scene_layer_add_mesh(&scene->layer.lit, new_mesh);
}

mesh *scene_new_mesh_unlit(scene *scene) {
  mesh *new_mesh = scene_new_mesh(scene);
  return scene_layer_add_mesh(&scene->layer.unlit, new_mesh);
}

void scene_draw(scene *scene, MeshDrawMethod draw_method,
                WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(&scene->camera);

  // draw different scene mesh list depending on defined draw method
  switch (draw_method) {

    // only draw lit mesh if draw method is Shadow
  case MESH_SHADER_SHADOW:
    scene_draw_mesh_list(scene, draw_method, render_pass, &scene->layer.lit);
    break;

  case MESH_SHADER_DEFAULT:
  case MESH_SHADER_SOLID:
  case MESH_SHADER_WIREFRAME:
  case MESH_SHADER_CUSTOM:
  default:
    // draw solid meshes first
    scene_draw_mesh_list(scene, draw_method, render_pass, &scene->layer.lit);
    // draw transparent meshes then
    scene_draw_mesh_list(scene, draw_method, render_pass, &scene->layer.unlit);
  }
}

/**
   Build meshes shader in each scene list
 */
void scene_build(scene *scene, MeshDrawMethod draw_method) {

  printf("======= BUILD SCENE ======\n");
  // Build shader (establish pipeline from previously set bind groups)

  // draw solid meshes first
  scene_build_mesh_list(scene, draw_method, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, draw_method, &scene->layer.unlit);
}

void scene_build_mesh_list(scene *scene, MeshDrawMethod draw_method,
                           MeshIndexedList *mesh_list) {

  for (int i = 0; i < mesh_list->length; i++) {
    mesh *current_mesh = mesh_list->entries[i];
    mesh_build(current_mesh, draw_method);
    shader_module_release(mesh_shader_texture(current_mesh));
  }
}

/**
   Add a mesh pointer from the global array to the indexed list (scene layer)
 */
static mesh *scene_layer_add_mesh(MeshIndexedList *mesh_list, mesh *new_mesh) {
  // ADD MESH TO LIST
  // eventually expand mesh vector if overflow
  if (mesh_list->length == mesh_list->capacity) {
    size_t new_capacity = mesh_list->capacity * 2;
    mesh **temp = realloc(mesh_list->entries, sizeof(mesh *) * new_capacity);

    if (temp) {
      mesh_list->entries = temp;
      mesh_list->capacity = new_capacity;
    } else {
      VERBOSE_PRINT("Scene mesh list reached full capacity, could not "
                    "reallocate new space\n");
      return 0;
    }
  }

  mesh_list->entries[mesh_list->length] = new_mesh;
  mesh_list->length++;
  return new_mesh;
}

mesh *scene_new_mesh(scene *scene) {

  if (scene->meshes.length == scene->meshes.capacity) {
    size_t new_capacity = scene->meshes.capacity * 2;
    mesh *temp = realloc(scene->meshes.entries, sizeof(mesh) * new_capacity);

    if (temp) {
      scene->meshes.entries = temp;
      scene->meshes.capacity = new_capacity;
    } else {
      VERBOSE_PRINT("Scene mesh list reached full capacity, could not "
                    "reallocate new space\n");
      return 0;
    }
  }

  return &scene->meshes.entries[scene->meshes.length++];
}

void scene_draw_mesh_list(scene *scene, MeshDrawMethod draw_method,
                          WGPURenderPassEncoder *render_pass,
                          MeshIndexedList *mesh_list) {

  // loop through mesh list and draw meshes
  for (int i = 0; i < mesh_list->length; i++) {
    mesh *current_mesh = mesh_list->entries[i];
    mesh_draw(current_mesh, draw_method, render_pass, &scene->camera,
              &scene->viewport);
  }
}

void scene_init_mesh_layer(MeshIndexedList *mesh_list) {

  mesh_list->entries = malloc(SCENE_MESH_LIST_DEFAULT_CAPACITY * sizeof(mesh));
  mesh_list->length = 0;
  mesh_list->capacity = SCENE_MESH_LIST_DEFAULT_CAPACITY;
}

void scene_init_light_list(scene *scene) {

  // init point light list
  scene->lights.point.capacity = LIGHT_MAX_CAPACITY;
  scene->lights.point.length = 0;

  // init spot light list
  scene->lights.spot.capacity = LIGHT_MAX_CAPACITY;
  scene->lights.spot.length = 0;

  // init ambient light list
  scene->lights.ambient.capacity = LIGHT_MAX_CAPACITY;
  scene->lights.ambient.length = 0;

  // init ambient light list
  scene->lights.sun.capacity = LIGHT_MAX_CAPACITY;
  scene->lights.sun.length = 0;
}

size_t scene_add_point_light(scene *scene, PointLightDescriptor *desc) {

  PointLightList *list = &scene->lights.point;
  if (list->length == list->capacity) {
    perror("Scene point light capacity reached maximum");
    return 0;
  }

  PointLight *new_light = &list->entries[list->length++];
  light_create_point(new_light, desc);

  return list->length;
}

size_t scene_add_spot_light(scene *scene, SpotLightDescriptor *desc) {

  SpotLightList *list = &scene->lights.spot;
  if (list->length == list->capacity) {
    perror("Scene spot light capacity reached maximum");
    return 0;
  }

  SpotLight *new_light = &list->entries[list->length++];
  light_create_spot(new_light, desc);

  return list->length;
}

size_t scene_add_ambient_light(scene *scene, AmbientLightDescriptor *desc) {

  AmbientLightList *list = &scene->lights.ambient;
  if (list->length == list->capacity) {
    perror("Scene ambient light capacity reached maximum");
    return 0;
  }

  AmbientLight *new_light = &list->entries[list->length++];
  light_create_ambient(new_light, desc);

  return list->length;
}

size_t scene_add_sun_light(scene *scene, SunLightDescriptor *desc) {

  SunLightList *list = &scene->lights.sun;
  if (list->length == list->capacity) {
    perror("Scene sun light capacity reached maximum");
    return 0;
  }

  SunLight *new_light = &list->entries[list->length++];
  light_create_sun(new_light, desc);

  return list->length;
}
