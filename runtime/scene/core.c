#include "core.h"
#include "./editor/selection.h"
#include "editor/editor.h"
#include "layer.h"

static void scene_init_light_list(Scene *);
static Mesh *scene_new_mesh(Scene *, const char *);
static Camera *scene_init_main_camera(Scene *, cclock *);

void scene_create(Scene *scene, const SceneCreateDescriptor *desc) {

  scene->id = reg_register((void *)scene, RegEntryType_Scene);

  // create camera list, and set active camera
  camera_list_create(&scene->cameras, SCENE_CAMERA_LIST_CAPACITY);
  scene->camera = scene_init_main_camera(scene, desc->clock);
  scene->active_camera = scene->camera;

  // set wgpu related handles
  scene->device = desc->device;
  scene->queue = desc->queue;

  // set viewport
  viewport_create(&scene->viewport, desc->viewport);

  // init global mesh list
  mesh_list_create(&scene->meshes, SCENE_MESH_MAX_MESH_CAPACITY);

  // init mesh pipelines
  mesh_reference_list_create(&scene->pipelines.background,
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);
  mesh_reference_list_create(&scene->pipelines.lit,
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);
  mesh_reference_list_create(&scene->pipelines.unlit,
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);
  mesh_reference_list_create(&scene->pipelines.fixed,
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // init scene layers
  scene_layer_set_create(&scene->layers, SCENE_LAYER_SET_CAPACITY);

  // init lights
  scene_init_light_list(scene);

  /* ==== EDITOR ==== */
  // EDITORONLY
  scene_editor_init(scene);
 
}

/**
   Define scene main edit camera
 */
Camera *scene_init_main_camera(Scene *scene, cclock *clock) {

  Camera camera;

  // create main camera
  camera_create(&camera, &(CameraCreateDescriptor){
                             .speed = 20.0f,
                             .clock = clock,
                             .mode = CameraMode_Edit,
                             .sensitivity =
                                 {
                                     .move = 0.02f,
                                     .rotate = 0.002f,
                                     .zoom = 0.02f,
                                 },

                         });

  // init main camera position
  camera_lookat(&camera, (vec3){20.0f, 20.0f, 20.0f}, (vec3){0.0f, 0.0f, 0.0f});

  return camera_list_insert(&scene->cameras, &camera);
}

/**
 Return the new mesh pointer from the global array and push the new pointer to
 the right scene layer.
  1. first create new mesh in the scene pool
  2. add the reference to the relative mesh ref list
 */
Mesh *scene_new_mesh_lit(Scene *scene, const char *layer) {
  Mesh *new_mesh = scene_new_mesh(scene, layer);

  // return pipeline pointer (same as new_mesh)
  return mesh_reference_list_insert(&scene->pipelines.lit, new_mesh);
}

Mesh *scene_new_mesh_unlit(Scene *scene, const char *layer) {
  Mesh *new_mesh = scene_new_mesh(scene, layer);

  // return pipeline pointer (same as new_mesh)
  return mesh_reference_list_insert(&scene->pipelines.unlit, new_mesh);
}

Mesh *scene_new_mesh_fixed(Scene *scene, const char *layer) {
  Mesh *new_mesh = scene_new_mesh(scene, layer);

  // return pipeline pointer (same as new_mesh)
  return mesh_reference_list_insert(&scene->pipelines.fixed, new_mesh);
}

Mesh *scene_new_mesh_background(Scene *scene, const char *layer) {
  Mesh *new_mesh = scene_new_mesh(scene, layer);

  // return pipeline pointer (same as new_mesh)
  return mesh_reference_list_insert(&scene->pipelines.background, new_mesh);
}

Mesh *scene_new_mesh(Scene *scene, const char *layer) {
  Mesh *new_mesh = mesh_list_new_mesh(&scene->meshes);

  // add to scene layers ('Default' layer if NULL)
  if (layer == NULL)
    layer = SCENE_LAYER_DEFAULT;

  scene_layer_set_insert_mesh(&scene->layers, layer, new_mesh);

  return new_mesh;
}

/**
   Quick access to a scene layer mesh list.
 */
MeshRefList *scene_layer_meshes(Scene *scene, const char *name) {

  SceneLayer *layer = scene_layer_set_find(&scene->layers, name);

  if (layer == NULL)
    return NULL;

  return &layer->meshes;
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

/**
   Return pointer to scene mesh pool
 */
MeshList *scene_mesh_list(Scene *scene) { return &scene->meshes; }
