#include "ao.h"

#include "backend/ao_bake/core.h"
#include "backend/context.h"
#include "backend/renderer/core.h"
#include "runtime/scene/core.h"
#include "utils/color.h"

void example_ao(Scene *scene, Renderer *renderer, bool debug) {

  AOBakeDrawDebug debug_options = {0};

  if (debug) {
    debug_options = (AOBakeDrawDebug){
        .debug_scene = &scene->debug,
        .color = &(color){0.0f, 1.0f, 0.0f, 1.0f},
        .max_ray = 20,
    };
  }

  ao_bake_draw_list(&renderer->texture.ambient_occlusion,
                    &(AOBakeDrawDescriptor){
                        .mesh_list = renderer_pipeline(
                            renderer, RendererPipeline_Dynamic_LitShadow),
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
