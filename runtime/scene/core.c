#include "core.h"
#include "./draw.h"
#include "./draw_config.h"
#include "./editor/editor.h"
#include "./layer.h"
#include "debug/core.h"
#include "event/event.html.h"

#include "../utils/system.h"
#include "renderer/render_pass/core.h"

// initializers
static inline Camera *scene_init_main_camera(Scene *, cclock *);
static inline void scene_light_list_init(Scene *);
static inline void scene_camera_init(Scene *);
static inline void scene_probe_reflection_init(Scene *,
                                               const PipelineMultisampleCount);

static inline void scene_mesh_list_init(Scene *);

void scene_create(Scene *scene, const SceneCreateDescriptor *desc) {

  TIMER("Scene Load", {
    scene->id = reg_register((void *)scene, RegEntryType_Scene);

    {
      /*  ===== SCENE RENDER =====   */
      scene_renderer_init(&scene->renderer, desc->renderer);
      scene_environment_init(&scene->environment,
                             &(SceneEnvironmentDescriptor){
                                 .ssbo = &scene->renderer.ssbo,
                                 .ubo = &scene->renderer.ubo,
                             });
    }

    {
      /*  ===== LISTS ===== */
      scene_mesh_list_init(scene);
      scene_layer_init(&scene->layers);
      scene_light_list_init(scene);
      scene_probe_reflection_init(scene, desc->renderer->multisampling_count);
    }

    {
      /*  ===== CAMERA & VIEWPORT =====  */
      scene_camera_init(scene);

      viewport_create(&scene->viewport,
                      &(ViewportCreateDescriptor){
                          .fov = desc->viewport->fov,
                          .near_clip = desc->viewport->near_clip,
                          .far_clip = desc->viewport->far_clip,
                          .aspect = desc->viewport->aspect,
                          .width = scene_renderer_width(&scene->renderer),
                          .height = scene_renderer_height(&scene->renderer),
                      });

      ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_Viewport,
                      &scene->viewport.ssbo_slot);
    }

    {
      /*  ===== EDITOR =====  */
      scene_editor_init(scene); // EDITORONLY

      scene_debug_init(&scene->debug, &(SceneDebugDescriptor){
                                          .camera = scene->active_camera,
                                          .device = scene_device(scene),
                                          .queue = scene_queue(scene),
                                          .viewport = &scene->viewport,
                                          .pool = &scene->meshes,
                                          .ssbo = &scene->renderer.ssbo,
                                      });
    }

    {
      /*  ===== EVENT =====  */
      scene_event_html(scene);
      scene_draw_layouts_init(scene, desc->renderer->multisampling_count);
    }
  });
}

/**
   Initialize scene mesh pool as well as pipelines
 */
void scene_mesh_list_init(Scene *scene) {

  // init mesh pipelines
  for (ScenePipeline flag = 1; flag < (1 << SCENE_PIPELINE_COUNT); flag <<= 1)
    mesh_ref_list_create(scene_pipeline(scene, flag),
                         SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // init pool
  mesh_list_create(&scene->meshes, SCENE_MESH_MAX_MESH_CAPACITY);
}

/**
   Create scene camera list and main camera.
 */
void scene_camera_init(Scene *scene) {

  // create camera list, and set active camera
  camera_list_create(&scene->cameras, SCENE_CAMERA_LIST_CAPACITY);
  scene->camera =
      scene_init_main_camera(scene, scene_renderer_clock(&scene->renderer));

  ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_Camera,
                  &scene->camera->ssbo_slot);

  // set scene main camera as active
  scene->active_camera = scene->camera;
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

void scene_probe_reflection_init(Scene *scene,
                                 const PipelineMultisampleCount multisample) {

  const ScenePipeline reflection_pipelines[2] = {
      ScenePipeline_Dynamic_LitShadow,
      ScenePipeline_Dynamic_Lit,
  };

  RenderPassDrawListDescriptor reflection_draw_list = {.length = 2};
  for (uint8_t i = 0; i < 2; i++)
    reflection_draw_list.entries[i] = (RenderPassDrawLayoutDescriptor){
        .shader = MeshShader_Reflection,
        .topology_callback = mesh_topology_base,
        .meshes = scene_pipeline(scene, reflection_pipelines[i]),
        .mesh_preprocessor_callback = probe_reflection_list_draw_preprocessor,
    };

  ProbeReflectionListDescriptor reflection_config = {
      .capacity = PROBE_REFLECTION_GRID_LIST_CAPACITY,
      .device = scene_device(scene),
      .queue = scene_queue(scene),
      .multisample = multisample,
      .resolution = TextureResolution_512,
      .draw_list = &reflection_draw_list,
  };

  probe_reflection_grid_list_create(&scene->probes_reflection,
                                    &reflection_config);

  probe_reflection_plane_list_create(&scene->planes_reflection,
                                     &reflection_config);
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

void scene_light_list_init(Scene *scene) {

  light_list_create(&scene->lights, LIGHT_MAX_CAPACITY);

  // init shadow textures
  shadow_map_init(&(ShadowMapInitDescriptor){
      .device = scene_renderer_device(&scene->renderer),
      .queue = scene_renderer_queue(&scene->renderer),
      .lights = &scene->lights,
      .draw_list =
          &(RenderPassDrawListDescriptor){
              .length = 1,
              .entries =
                  {
                      {
                          .shader = MeshShader_Shadow,
                          .topology_callback = mesh_topology_base,
                          .mesh_preprocessor_callback =
                              shadow_map_pass_preprocessor_callback,
                          .mesh_preprocessor_data = (void *)NULL,
                          .meshes = scene_pipeline(
                              scene, ScenePipeline_Dynamic_LitShadow),
                      },
                  },
          },
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
