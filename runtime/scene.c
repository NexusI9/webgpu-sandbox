#include "scene.h"
#include "../utils/system.h"
#include "camera.h"
#include "gizmo.h"
#include "light.h"
#include "mesh.h"
#include "shader.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <assert.h>
#include <stdio.h>

static void scene_init_light_list(Scene *);
static MeshRefList *scene_layer_gizmo(Scene *);
static Mesh *scene_new_mesh(Scene *);
static void scene_draw_mesh_list(Scene *, mesh_get_vertex_callback,
                                 mesh_get_shader_callback,
                                 WGPURenderPassEncoder *, MeshRefList *);
static void scene_build_mesh_list(Scene *, mesh_get_shader_callback,
                                  MeshRefList *);

Scene scene_create(Camera camera, Viewport viewport) {

  Scene scene;

  // create camera list, and set active camera
  camera_list_create(&scene.cameras, SCENE_CAMERA_LIST_CAPACITY);
  scene.active_camera = camera_list_insert(&scene.cameras, &camera);

  // set viewport
  scene.viewport = viewport;

  // init global mesh list
  mesh_list_create(&scene.meshes, SCENE_MESH_MAX_MESH_CAPACITY);

  // init mesh layers
  mesh_reference_list_create(&scene.layer.lit, SCENE_MESH_LIST_DEFAULT_CAPACITY);
  mesh_reference_list_create(&scene.layer.unlit,
                           SCENE_MESH_LIST_DEFAULT_CAPACITY);
  mesh_reference_list_create(&scene.layer.fixed,
                           SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // init gizmo list
  gizmo_list_create(&scene.gizmo, GIZMO_LIST_CAPACITY_DEFAULT);

  // init lights
  scene_init_light_list(&scene);

  return scene;
}

/**
 Return the new mesh pointer from the global array and push the new pointer to
 the right scene layer
 */
Mesh *scene_new_mesh_lit(Scene *scene) {
  Mesh *new_mesh = scene_new_mesh(scene);
  return mesh_reference_list_insert(&scene->layer.lit, new_mesh);
}

Mesh *scene_new_mesh_unlit(Scene *scene) {
  Mesh *new_mesh = scene_new_mesh(scene);
  return mesh_reference_list_insert(&scene->layer.unlit, new_mesh);
}

Mesh *scene_new_mesh_fixed(Scene *scene) {
  Mesh *new_mesh = scene_new_mesh(scene);
  return mesh_reference_list_insert(&scene->layer.fixed, new_mesh);
}

// TODO: Simplify the overall scene draw/build process
void scene_draw_texture(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw solid meshes first
  scene_draw_mesh_list(scene, mesh_vertex_base, mesh_shader_texture,
                       render_pass, &scene->layer.lit);
  // draw transparent meshes then
  scene_draw_mesh_list(scene, mesh_vertex_base, mesh_shader_texture,
                       render_pass, &scene->layer.unlit);
}

void scene_draw_fixed(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw fixed mesh
  scene_draw_mesh_list(scene, mesh_vertex_base, mesh_shader_texture,
                       render_pass, &scene->layer.fixed);
}

void scene_draw_shadow(Scene *scene, WGPURenderPassEncoder *render_pass) {
  // onlid draw solid/lit meshes
  scene_draw_mesh_list(scene, mesh_vertex_base, mesh_shader_shadow, render_pass,
                       &scene->layer.lit);
}

void scene_draw_wireframe(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw solid meshes first
  scene_draw_mesh_list(scene, mesh_vertex_wireframe, mesh_shader_wireframe,
                       render_pass, &scene->layer.lit);
  // draw solid meshes then
  scene_draw_mesh_list(scene, mesh_vertex_wireframe, mesh_shader_wireframe,
                       render_pass, &scene->layer.unlit);
}

void scene_draw_solid(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw solid meshes first
  scene_draw_mesh_list(scene, mesh_vertex_base, mesh_shader_solid, render_pass,
                       &scene->layer.lit);
  // draw solid meshes then
  scene_draw_mesh_list(scene, mesh_vertex_base, mesh_shader_solid, render_pass,
                       &scene->layer.unlit);
}

/**
   Build meshes Texture shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_texture(Scene *scene) {
  VERBOSE_PRINT("======= BUILD TEXTURE SCENE ======\n");
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_texture, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_texture, &scene->layer.unlit);
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_solid(Scene *scene) {
  VERBOSE_PRINT("======= BUILD SOLID SCENE ======\n");
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_solid, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_solid, &scene->layer.unlit);
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_wireframe(Scene *scene) {
  VERBOSE_PRINT("======= BUILD WIREFRAME SCENE ======\n");
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_wireframe, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_wireframe, &scene->layer.unlit);
}

/**
   Build meshes Shadow shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_shadow(Scene *scene) {
  VERBOSE_PRINT("======= BUILD SHADOW SCENE ======\n");
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_shadow, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_shadow, &scene->layer.unlit);
}

/**
   Build Fixed mesh layer.
   Fixed layer use the Texture shader as default shader (preventing creating an
   additional persisten unsued Fixed Shader in Mesh).
 */
void scene_build_fixed(Scene *scene) {
  VERBOSE_PRINT("======= BUILD FIXED SCENE ======\n");
  scene_build_mesh_list(scene, mesh_shader_texture, &scene->layer.fixed);
}

void scene_build_mesh_list(Scene *scene, mesh_get_shader_callback target_shader,
                           MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];
    mesh_build(current_mesh, target_shader(current_mesh));
    shader_module_release(target_shader(current_mesh));
  }
}

Mesh *scene_new_mesh(Scene *scene) { return mesh_list_insert(&scene->meshes); }

void scene_draw_mesh_list(Scene *scene, mesh_get_vertex_callback target_vertex,
                          mesh_get_shader_callback target_shader,
                          WGPURenderPassEncoder *render_pass,
                          MeshRefList *mesh_list) {

  // loop through mesh list and draw meshes
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];
    mesh_draw(target_vertex(current_mesh), target_shader(current_mesh),
              render_pass, scene->active_camera, &scene->viewport);
  }
}

// TODO: move light list in Light not Scene anymore
void scene_init_light_list(Scene *scene) {

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

size_t scene_add_point_light(Scene *scene, PointLightDescriptor *desc,
                             WGPUDevice *device, WGPUQueue *queue) {

  PointLightList *list = &scene->lights.point;
  if (list->length == list->capacity) {
    perror("Scene point light capacity reached maximum");
    return 0;
  }

  // create point light
  PointLight *new_light = &list->entries[list->length++];
  light_create_point(new_light, desc);

  // create mesh/gizmo
  MeshList *mesh_list = scene_mesh_list(scene);
  MeshRefList cache_list;
  mesh_reference_list_create(&cache_list, 1);
  MeshRefList *render_list = scene_layer_gizmo(scene);

  light_point_create_mesh(new_light, &(LightCreateMeshDescriptor){
                                         .camera = scene->active_camera,
                                         .viewport = &scene->viewport,
                                         .device = device,
                                         .queue = queue,
                                         .list = mesh_list,
                                         .destination = &cache_list,
                                     });

  // transfert gizmo mesh pointers to render_list
  mesh_reference_list_transfert(&cache_list, render_list);

  // add gizmo to gizmo list
  //gizmo_list_insert_point_light(&scene->gizmo, new_light, );

  return list->length;
}

size_t scene_add_spot_light(Scene *scene, SpotLightDescriptor *desc,
                            WGPUDevice *device, WGPUQueue *queue) {

  SpotLightList *list = &scene->lights.spot;
  if (list->length == list->capacity) {
    perror("Scene spot light capacity reached maximum");
    return 0;
  }

  // create spot light
  SpotLight *new_light = &list->entries[list->length++];
  light_create_spot(new_light, desc);

  // create mesh/gizmo
  MeshList *mesh_list = scene_mesh_list(scene);
  MeshRefList cache_list;
  mesh_reference_list_create(&cache_list, 1);
  MeshRefList *render_list = scene_layer_gizmo(scene);

  light_spot_create_mesh(new_light, &(LightCreateMeshDescriptor){
                                        .camera = scene->active_camera,
                                        .viewport = &scene->viewport,
                                        .device = device,
                                        .queue = queue,
                                        .list = mesh_list,
                                        .destination = &cache_list,
                                    });

  // transfert gizmo mesh pointers to render_list
  mesh_reference_list_transfert(&cache_list, render_list);

  return list->length;
}

size_t scene_add_ambient_light(Scene *scene, AmbientLightDescriptor *desc,
                               WGPUDevice *device, WGPUQueue *queue) {

  AmbientLightList *list = &scene->lights.ambient;
  if (list->length == list->capacity) {
    perror("Scene ambient light capacity reached maximum");
    return 0;
  }

  // create ambient light
  AmbientLight *new_light = &list->entries[list->length++];
  light_create_ambient(new_light, desc);

  // create mesh/gizmo
  MeshList *mesh_list = scene_mesh_list(scene);
  MeshRefList cache_list;
  mesh_reference_list_create(&cache_list, 1);
  MeshRefList *render_list = scene_layer_gizmo(scene);

  light_ambient_create_mesh(new_light, &(LightCreateMeshDescriptor){
                                           .camera = scene->active_camera,
                                           .viewport = &scene->viewport,
                                           .device = device,
                                           .queue = queue,
                                           .list = mesh_list,
                                           .destination = &cache_list,
                                       });

  // transfert gizmo mesh pointers to render_list
  mesh_reference_list_transfert(&cache_list, render_list);

  return list->length;
}

size_t scene_add_sun_light(Scene *scene, SunLightDescriptor *desc,
                           WGPUDevice *device, WGPUQueue *queue) {

  SunLightList *list = &scene->lights.sun;
  if (list->length == list->capacity) {
    perror("Scene sun light capacity reached maximum");
    return 0;
  }

  // create sun light
  SunLight *new_light = &list->entries[list->length++];
  light_create_sun(new_light, desc);

  // create mesh/gizmo
  MeshList *mesh_list = scene_mesh_list(scene);
  MeshRefList cache_list;
  mesh_reference_list_create(&cache_list, 1);
  MeshRefList *render_list = scene_layer_gizmo(scene);

  light_sun_create_mesh(new_light, &(LightCreateMeshDescriptor){
                                       .camera = scene->active_camera,
                                       .viewport = &scene->viewport,
                                       .device = device,
                                       .queue = queue,
                                       .list = mesh_list,
                                       .destination = &cache_list,
                                   });

  // transfert gizmo mesh pointers to render_list
  mesh_reference_list_transfert(&cache_list, render_list);

  return list->length;
}

/**
   Return pointer to mesh gizmo layer ("Fixed" layer)
 */
MeshRefList *scene_layer_gizmo(Scene *scene) { return &scene->layer.fixed; }

/**
   Return pointer to scene mesh pool
 */
MeshList *scene_mesh_list(Scene *scene) { return &scene->meshes; }

/**
   "Create" a new uninitialized camera in the scene camera list and return the
   newly created item's pointer.
 */
Camera *scene_new_camera(Scene *scene) {
  return camera_list_new_camera(&scene->cameras);
}
