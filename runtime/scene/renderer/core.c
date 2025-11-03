#include "core.h"

#include <emscripten/emscripten.h>
#include <stdint.h>
#include <string.h>

#include "backend/ao_bake/core.h"
#include "backend/clock.h"
#include "backend/compute/core.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/postfx/core.h"
#include "backend/profiler.h"
#include "backend/stat.h"
#include "backend/std_pipeline/core.h"
#include "backend/std_texture/core.h"
#include "backend/ubo.h"
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include "render_pass/draw.h"
#include "runtime/input/core.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"

static void scene_renderer_render(void *);

void scene_renderer_init(SceneRenderer *renderer,
                         const SceneRendererCreateDescriptor *rd) {

  renderer->context.background = rd->background;
  renderer->context.width = rd->width ? rd->width : context_width();
  renderer->context.height = rd->height ? rd->height : context_height();
  renderer->context.dpi = rd->dpi == SCENE_RENDERER_DPI_AUTO
                              ? emscripten_get_device_pixel_ratio()
                              : rd->dpi;
  renderer->draw.mode = SceneRendererDrawMode_Solid;

  clock_init(&renderer->clock);
  profiler_init(&renderer->profiler);
  stat_init(&renderer->stats);

  TIMER("AO Bake", {
    ao_bake_init(&renderer->texture.ambient_occlusion,
                 &(AOBakeInitDescriptor){
                     .size = AO_TEXTURE_RESOLUTION,
                     .layer_count = AO_LAYER_COUNT,
                 });
  });

  {
    // init various buffers
    compute_pass_init(&renderer->draw.compute_pass,
                      &(ComputePassDescriptor){
                          .max_height = context_height(),
                          .max_width = context_width(),
                      });

    ubo_init(&renderer->ubo);
  }

  scene_renderer_add_draw_callback(
      renderer, ubo_draw_callback, (void *)&renderer->ubo,
      SceneRendererDrawMode_Texture | SceneRendererDrawMode_Solid |
          SceneRendererDrawMode_Wireframe | SceneRendererDrawMode_Boundbox);
}

/**
   Based on the renderer Draw Layouts, it first select the entry base on the
   renderer mode (texture/solid/wireframe).
 */
void scene_renderer_draw_layout_callback(void *data) {
  SceneRenderer *renderer = (SceneRenderer *)data;

  // retrieve render mode
  const SceneRendererDrawMode mode = renderer->draw.mode;
  render_pass_list_draw(&renderer->draw.render_pass[__builtin_ctz(mode)]);
}

/**
   Draw callbackas are basically list of functions that will be called during
   the draw loop. Those hooks accept additional user data in argument. A current
   example of hooks are:
   [
     update_camera_matrix(),
     draw_pipelines()
     ]

   Note that the user data longevity is not handled by the hook, meaning it's
   the developper responsibility to manage the lifecycle of the data
   (allocating, freeing...)
 */
void scene_renderer_add_draw_callback(SceneRenderer *renderer,
                                      scene_renderer_draw_callback callback,
                                      void *data, const int modes) {

  // add hook to corressponding mode
  for (uint8_t i = 0; i < SCENE_RENDERER_DRAW_MODE_COUNT; i++) {
    if (modes & (1 << i)) {

      // do not add if max hook reached
      if (renderer->draw.callbacks[i].length == SCENE_RENDERER_MAX_HOOK) {
        logger_add(LoggerFlag_Warning, "Max draw hook reached.\n");
        return;
      }

      renderer->draw.callbacks[i]
          .entries[renderer->draw.callbacks[i].length++] =
          (SceneRendererDrawCallback){callback, data};
    }
  }
}

/**
   Renderer Main Loop, basically just loop through the registered callbacks.
 */
void scene_renderer_render(void *desc) {
  SceneRendererRenderDescriptor *config = (SceneRendererRenderDescriptor *)desc;

  profiler_latency_end(&config->renderer->profiler,
                       ProfilerLatencyType_MainLoop);
  profiler_latency_start(&config->renderer->profiler,
                         ProfilerLatencyType_MainLoop);

  // Call draw callbacks of active renderere draw mode
  SceneRendererDrawCallbackList *callback_list =
      &config->renderer->draw
           .callbacks[__builtin_ctz(config->renderer->draw.mode)];

  // call callbacks, pass renderer and data
  for (size_t i = 0; i < callback_list->length; i++) {
    SceneRendererDrawCallback *cb = &callback_list->entries[i];
    cb->callback(cb->data);
  }

  // update clock delta
  clock_update_delta(&config->renderer->clock);
}

/**
   Draw a scene with a specified draw mode along with the render pass that comes
   with it (ao, shadow mapping...). Also call the main loop.
 */
void scene_renderer_draw(SceneRenderer *renderer) {
  // call main loop
  emscripten_set_main_loop_arg(
      scene_renderer_render,
      &(SceneRendererRenderDescriptor){.renderer = renderer}, 0, 1);
}

// getters
void scene_renderer_set_draw_mode(SceneRenderer *renderer,
                                  const SceneRendererDrawMode mode) {
  renderer->draw.mode = mode;
}
