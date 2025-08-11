#include "core.h"
#include "./draw.h"
#include "./editor/editor.h"
#include "./layer.h"
#include "event/event.html.h"
#include "./draw_config.h"

// initializers
static inline Camera *scene_init_main_camera(Scene *, cclock *);
static inline void scene_init_light_list(Scene *);
static inline void scene_init_camera(Scene *);

void scene_create(Scene *scene, const SceneCreateDescriptor *desc) {

  scene->id = reg_register((void *)scene, RegEntryType_Scene);

  /*

    ===== SCENE RENDER =====

   */

  // init mesh pipelines
  for (ScenePipeline flag = 1; flag < (1 << SCENE_PIPELINE_COUNT); flag <<= 1)
    mesh_ref_list_create(scene_pipeline(scene, flag),
                         SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // init draw callbacks configuration
  scene_init_draw_layouts(scene);

  // set renderer
  scene_renderer_create(&scene->renderer, desc->renderer);

  /*

    ===== MESH LISTS =====

   */

  // init scene layers
  scene_layer_init(&scene->layers);

  // init lights
  scene_init_light_list(scene);

  // init global mesh list
  mesh_list_create(&scene->meshes, SCENE_MESH_MAX_MESH_CAPACITY);

  /*

    ===== CAMERA & VIEWPORT =====

   */

  // init camera lists and set main/active camera
  scene_init_camera(scene);

  // set viewport (using renderer width/height)
  viewport_create(&scene->viewport,
                  &(ViewportCreateDescriptor){
                      .fov = desc->viewport->fov,
                      .near_clip = desc->viewport->near_clip,
                      .far_clip = desc->viewport->far_clip,
                      .aspect = desc->viewport->aspect,
                      .width = scene_renderer_width(&scene->renderer),
                      .height = scene_renderer_height(&scene->renderer),
                  });

  /*

  ===== EVENT =====

   */

  // scene_event_html(scene);

  /*

    ===== EDITOR =====

   */
  // EDITORONLY
  scene_editor_init(scene);
}

/**
   Create scene camera list and main camera.
 */
void scene_init_camera(Scene *scene) {

  // create camera list, and set active camera
  camera_list_create(&scene->cameras, SCENE_CAMERA_LIST_CAPACITY);
  scene->camera =
      scene_init_main_camera(scene, scene_renderer_clock(&scene->renderer));

  // set scene main camera as active
  scene->active_camera = scene->camera;

  // add the camera update callback
  scene_renderer_add_draw_callback(&scene->renderer, scene_camera_draw_callback,
                                   (void *)scene->active_camera);
}

/**
   Define scene main edit camera
 */
Camera *scene_init_main_camera(Scene *scene, cclock *clock) {

  Camera *camera = camera_list_new_camera(&scene->cameras);

  // create main camera
  camera_create(camera, &(CameraCreateDescriptor){
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
  camera_lookat(camera, (vec3){20.0f, 20.0f, 20.0f}, (vec3){0.0f, 0.0f, 0.0f});

  return camera;
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

  // init shadow textures
  shadow_map_init(&(ShadowMapInitDescriptor){
      .device = scene_renderer_device(&scene->renderer),
      .queue = scene_renderer_queue(&scene->renderer),
      .lights = &scene->lights,
  });
}

/**
   Return pointer to scene mesh pool
 */
MeshList *scene_mesh_list(Scene *scene) { return &scene->meshes; }

/**
   Return nested queue from the scene renderer
 */
WGPUQueue scene_queue(Scene *scene) { return scene->renderer.wgpu.queue; }

/**
   Return nested device from the scene renderer
 */
WGPUDevice scene_device(Scene *scene) { return scene->renderer.wgpu.device; }
