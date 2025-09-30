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
#include "backend/ssbo.h"
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
static double scene_renderer_dpi(double);

void scene_renderer_init(SceneRenderer *renderer,
                         const SceneRendererCreateDescriptor *rd) {

  renderer->background = rd->background;
  renderer->context.dpi = scene_renderer_dpi(rd->dpi);

  // create clock
  clock_create(&renderer->clock);

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

    ssbo_init(&renderer->ssbo);
  }

  scene_renderer_add_draw_callback(
      renderer, ssbo_draw_callback, (void *)&renderer->ssbo,
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

double scene_renderer_dpi(double value) {
  // request dpi
  if (value == SCENE_RENDERER_DPI_AUTO)
    return emscripten_get_device_pixel_ratio();

  return value;
}

void scene_renderer_close(const SceneRenderer *renderer) {
  wgpuRenderPipelineRelease(renderer->wgpu.pipeline);
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
                                      void *data,
                                      const SceneRendererDrawMode modes) {

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

void scene_renderer_render(void *desc) {
  SceneRendererRenderDescriptor *config = (SceneRendererRenderDescriptor *)desc;

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
