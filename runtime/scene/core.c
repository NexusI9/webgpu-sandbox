#include "core.h"

static void scene_init_light_list(Scene *);
static Mesh *scene_new_mesh(Scene *);
static Camera *scene_init_main_camera(Scene *, cclock *);

void scene_create(Scene *scene, cclock *clock, Viewport viewport) {

  // create camera list, and set active camera
  camera_list_create(&scene->cameras, SCENE_CAMERA_LIST_CAPACITY);
  scene->camera = scene_init_main_camera(scene, clock);
  scene->active_camera = scene->camera;

  // set viewport
  scene->viewport = viewport;

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

  // init gizmo list
  gizmo_list_create(&scene->gizmo, GIZMO_LIST_CAPACITY_DEFAULT);

  // init lights
  scene_init_light_list(scene);

  scene->id = reg_register((void *)scene, RegEntryType_Scene);
}

/**
   Define scene main edit camera
 */
Camera *scene_init_main_camera(Scene *scene, cclock *clock) {

  Camera camera;
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

  return camera_list_insert(&scene->cameras, &camera);
}

/**
 Return the new mesh pointer from the global array and push the new pointer to
 the right scene layer.
  1. first create new mesh in the scene pool
  2. add the reference to the relative mesh ref list
 */
Mesh *scene_new_mesh_lit(Scene *scene) {
  Mesh *new_mesh = scene_new_mesh(scene);
  return mesh_reference_list_insert(&scene->pipelines.lit, new_mesh);
}

Mesh *scene_new_mesh_unlit(Scene *scene) {
  Mesh *new_mesh = scene_new_mesh(scene);
  return mesh_reference_list_insert(&scene->pipelines.unlit, new_mesh);
}

Mesh *scene_new_mesh_fixed(Scene *scene) {
  Mesh *new_mesh = scene_new_mesh(scene);
  return mesh_reference_list_insert(&scene->pipelines.fixed, new_mesh);
}

Mesh *scene_new_mesh_background(Scene *scene) {
  Mesh *new_mesh = scene_new_mesh(scene);
  return mesh_reference_list_insert(&scene->pipelines.background, new_mesh);
}

Mesh *scene_new_mesh(Scene *scene) {
  return mesh_list_new_mesh(&scene->meshes);
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
