#include "core.h"
#include "./draw.h"
#include "./editor/editor.h"
#include "./editor/selection.h"
#include "./layer.h"

// initializers
static inline Camera *scene_init_main_camera(Scene *, cclock *);
static inline void scene_init_draw_layouts(Scene *);
static inline void scene_init_light_list(Scene *);

void scene_create(Scene *scene, const SceneCreateDescriptor *desc) {

  scene->id = reg_register((void *)scene, RegEntryType_Scene);

  // create camera list, and set active camera
  camera_list_create(&scene->cameras, SCENE_CAMERA_LIST_CAPACITY);
  scene->camera = scene_init_main_camera(scene, desc->clock);
  scene->active_camera = scene->camera;

  // set renderer
  scene_renderer_create(&scene->renderer, desc->renderer);

  // set viewport
  // TODO: currently it's kinda weird to include the width and height in the
  // viewport descriptor by override it in the scene. Maybe remove the width and
  // height from the create descriptor (although it seems counter intuitive to
  // do so...)
  viewport_create(&scene->viewport, desc->viewport);
  scene->viewport.width = scene_renderer_width(&scene->renderer);
  scene->viewport.height = scene_renderer_height(&scene->renderer);

  // init global mesh list
  mesh_list_create(&scene->meshes, SCENE_MESH_MAX_MESH_CAPACITY);

  // init mesh pipelines

  // background
  mesh_reference_list_create(&scene->pipelines[ScenePipeline_Background],
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);
  // lit
  mesh_reference_list_create(&scene->pipelines[ScenePipeline_Lit],
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);
  // unlit
  mesh_reference_list_create(&scene->pipelines[ScenePipeline_Unlit],
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);
  // fixed
  mesh_reference_list_create(&scene->pipelines[ScenePipeline_Fixed],
                             SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // init scene layers
  scene_layer_set_create(&scene->layers, SCENE_LAYER_SET_CAPACITY);

  // init lights
  scene_init_light_list(scene);

  // init draw callbacks configuration
  scene_init_draw_layouts(scene);

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
   Define the scene renderer draw configurations by providing each draw mode
   their respective topology, shader callbacks as well a mesh list to draw
   during the loop.
 */
void scene_init_draw_layouts(Scene *scene) {

  // Texture draw configuration
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Texture,
      &(SceneRendererDrawLayoutList){
          .length = 4,
          .entries =
              {
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Background],
                      .shader_callback = mesh_shader_texture,
                      .topology_callback = mesh_topology_base,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Lit],
                      .shader_callback = mesh_shader_texture,
                      .topology_callback = mesh_topology_base,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Unlit],
                      .shader_callback = mesh_shader_texture,
                      .topology_callback = mesh_topology_base,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Selection],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_boundbox,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Fixed],
                      .shader_callback = mesh_shader_override,
                      .topology_callback = mesh_topology_override,
                  },
              },
      });

  // Solid draw configuration
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Solid,
      &(SceneRendererDrawLayoutList){
          .length = 4,
          .entries =
              {
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Lit],
                      .shader_callback = mesh_shader_solid,
                      .topology_callback = mesh_topology_base,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Unlit],
                      .shader_callback = mesh_shader_solid,
                      .topology_callback = mesh_topology_base,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Selection],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_boundbox,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Fixed],
                      .shader_callback = mesh_shader_override,
                      .topology_callback = mesh_topology_override,
                  },

              },
      });

  // Wireframe draw configuration
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Wireframe,
      &(SceneRendererDrawLayoutList){
          .length = 4,
          .entries =
              {
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Lit],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_wireframe,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Unlit],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_wireframe,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Selection],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_boundbox,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Fixed],
                      .shader_callback = mesh_shader_override,
                      .topology_callback = mesh_topology_override,
                  },
              },
      });

  // Boundbox draw configuration
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Boundbox,
      &(SceneRendererDrawLayoutList){
          .length = 3,
          .entries =
              {
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Lit],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_boundbox,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Unlit],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_boundbox,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Selection],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_boundbox,
                  },
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Fixed],
                      .shader_callback = mesh_shader_override,
                      .topology_callback = mesh_topology_override,
                  },
              },
      });

  
  /*
    Below configuration won't be used in runtime out of debug purpose.
   */

  // Fixed draw configuration (use override topology & shader)
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Fixed,
      &(SceneRendererDrawLayoutList){
          .length = 1,
          .entries =
              {
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Fixed],
                      .shader_callback = mesh_shader_override,
                      .topology_callback = mesh_topology_override,
                  },
              },
      });

  // Selection draw configuration
  scene_renderer_set_draw_layout(
      &scene->renderer, SceneRendererDrawMode_Selection,
      &(SceneRendererDrawLayoutList){
          .length = 1,
          .entries =
              {
                  {
                      .meshes = &scene->pipelines[ScenePipeline_Selection],
                      .shader_callback = mesh_shader_wireframe,
                      .topology_callback = mesh_topology_boundbox,
                  },
              },
      });

  // add the camera update callback
  scene_renderer_add_draw_callback(&scene->renderer, scene_camera_draw_callback,
                                   (void *)scene->active_camera);

  // once defined, add layouts draw callbacks
  scene_renderer_add_draw_callback(&scene->renderer, scene_layout_draw_callback,
                                   (void *)&scene->renderer);
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

/**
   Return nested queue from the scene renderer
 */
WGPUQueue *scene_queue(Scene *scene) { return &scene->renderer.wgpu.queue; }

/**
   Return nested device from the scene renderer
 */
WGPUDevice *scene_device(Scene *scene) { return &scene->renderer.wgpu.device; }
