#include "ao.h"

void example_ao(Scene *scene, bool debug) {

  AOBakeDrawDebug debug_options = {0};

  if (debug) {
    debug_options = (AOBakeDrawDebug){
        .debug_scene = &scene->debug,
        .color = &(color){0.0f, 1.0f, 0.0f, 1.0f},
        .max_ray = 20,
    };
  }

  ao_bake_draw_list(
      &scene->renderer.texture.ambient_occlusion,
      &(AOBakeDrawDescriptor){
          .queue = scene_renderer_queue(&scene->renderer),
          .device = scene_renderer_device(&scene->renderer),
          .mesh_list = scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
          .global =
              {
                  AO_GLOBAL_RAY_AMOUNT,
                  AO_GLOBAL_RAY_MAX_DISTANCE,
              },
          .local =
              {
                  AO_LOCAL_RAY_AMOUNT,
                  AO_LOCAL_RAY_MAX_DISTANCE,
              },
          .debug = &debug_options,
      });
}
