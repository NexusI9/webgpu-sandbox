#include "core.h"
#include "../runtime/html_event/html_event.h"
#include "../runtime/input/input.h"
#include "../utils/system.h"
#include "ao_bake/core.h"
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include "render_pass.h"
#include "std_pipeline/core.h"
#include "std_texture/core.h"
#include "webgpu/webgpu.h"
#include <string.h>

static void scene_renderer_init(SceneRenderer *);

static inline void
scene_renderer_init_render_pass(SceneRenderer *,
                                const PipelineMultisampleCount);

static void scene_renderer_resize(SceneRenderer *);

static void scene_renderer_render(void *);

static double scene_renderer_dpi(double);

static inline WGPUSwapChain
scene_renderer_create_swapchain(const SceneRenderer *);

void scene_renderer_create(SceneRenderer *renderer,
                           const SceneRendererCreateDescriptor *rd) {

  renderer->context.name = rd->name;
  renderer->background = rd->background;
  renderer->context.dpi = scene_renderer_dpi(rd->dpi);

  // create clock
  clock_create(&renderer->clock);

  // set wgpu data
  renderer->wgpu.instance = wgpuCreateInstance(NULL);
  renderer->wgpu.device = emscripten_webgpu_get_device();
  renderer->wgpu.queue = wgpuDeviceGetQueue(renderer->wgpu.device);

  // define context size
  scene_renderer_resize(renderer);

  // init render passes
  scene_renderer_init_render_pass(renderer, rd->multisampling_count);

  TIMER("", {
    ao_bake_init(&renderer->texture.ambient_occlusion,
                 &(AOBakeInitDescriptor){
                     .size = AO_TEXTURE_RESOLUTION,
                     .layer_count = AO_LAYER_COUNT,
                     .device = scene_renderer_device(renderer),
                 });
  });

  // set draw layouts callback
  if (renderer->draw.layouts->length == 0) {
    VERBOSE_WARNING("No draw layouts were provided for the scene renderer.");
  } else {
    scene_renderer_add_draw_callback(
        renderer, scene_renderer_draw_layout_callback, (void *)renderer);
  }

  /*

     Global Input & Event polling

   */

  // TODO: Since renderer isn't high level anymore, put the below calls in a
  // more global object ("Context" ?)

  // set fallback textures

  TIMER("", {
    scene_renderer_init_fallback_textures(scene_renderer_device(renderer),
                                          scene_renderer_queue(renderer));
  });

  renderer->texture.skybox.cubemap =
      std_texture_view(TextureViewType_FloatCube);

  // init standards shaders
  standard_pipelines_init(scene_renderer_device(renderer),
                          rd->multisampling_count);

  // init global HTML event manager with context
  //  name (implicit)
  html_event_init(rd->name);

  // poll global input
  input_listen();

  /*********************************/

  // init resize event
  scene_renderer_init(renderer);
}

/**
   Create scene renderer draw config, which basically is an array of callback
   functions and mesh referecences list lists that will be picked during the
   draw loop.

   Basically for each draw call we require a "topology callback" and a
   "shader callback" to define which topology and shader we want to draw for
   each mesh.

   Note that the order of the array is relative to the SceneRendererMode:

   0 - Texture config
          L Render Pass 1
          L Render Pass 2
               L Length
               L Draw Layouts[]
                    L Mesh List
                    L Shader Callback
                    L Topo Callback

   1 - Solid config
   2 - Wireframe config
   3 - Boundbox config

   By following this order, we can simply map the right array entry depending on
   the scene render mode.
 */
void scene_renderer_set_draw_layout(SceneRenderer *renderer,
                                    const SceneRendererDrawMode mode,
                                    const RenderPassLayout *pass_layout) {

  if (mode >= SCENE_RENDERER_DRAW_MODE_COUNT)
    return;

  for (size_t i = 0; i < pass_layout->length; i++) {

    const RenderPassDrawList *render_pass = &pass_layout->entries[i];

    renderer->draw.layouts[mode].length = pass_layout->length;

    /* Map descriptor attribute to entity*/

    // target scene renderer based on mode (tex/solid/wire) and
    // type(Scene/Gizmo...)
    RenderPassDrawList *dest_layout =
        &renderer->draw.layouts[mode].entries[render_pass->pass];

    // assign length
    dest_layout->length = render_pass->length;
    dest_layout->pass = render_pass->pass;

    // copy mesh ref lists array
    memcpy(dest_layout->entries, render_pass->entries,
           sizeof(RenderPassDrawLayout) * dest_layout->length);
  }
}

/**
   Based on the renderer Draw Layouts, it first select the entry base on the
   renderer mode (texture/solid/wireframe).
 */
void scene_renderer_draw_layout_callback(void *data) {

  SceneRenderer *renderer = (SceneRenderer *)data;

  // retrieve render mode
  const SceneRendererDrawMode mode = renderer->draw.mode;

  // retrieve render pass layout related to the draw mode
  RenderPassLayout *pass_layout = &renderer->draw.layouts[mode];

  // Go through and draw each mode render pass
  render_pass_draw(renderer->draw.pass,
                   &(RenderPassDrawDescriptor){
                       .pass_layout = pass_layout,
                       .queue = scene_renderer_queue(renderer),
                       .device = scene_renderer_device(renderer),
                   });
}

bool scene_renderer_resize_callback(int event_type,
                                    const EmscriptenUiEvent *ui_event,
                                    void *user_data) {

  SceneRenderer *renderer = (SceneRenderer *)user_data;
  scene_renderer_resize(renderer);
  return 1;
}

void scene_renderer_resize(SceneRenderer *renderer) {

  double w, h;

  // retrieve canvas dimension
  emscripten_get_element_css_size(renderer->context.name, &w, &h);

  // define render resolution
  renderer->context.width = (int)w * renderer->context.dpi;
  renderer->context.height = (int)h * renderer->context.dpi;

  // set canvas size
  emscripten_set_element_css_size(renderer->context.name, w, h);

  // reset swap chain on resize
  if (renderer->draw.swapchain) {
    wgpuSwapChainRelease(renderer->draw.swapchain);
    renderer->draw.swapchain = NULL;
  }

  renderer->draw.swapchain = scene_renderer_create_swapchain(renderer);
}

double scene_renderer_dpi(double value) {

  // request dpi
  if (value == SCENE_RENDERER_DPI_AUTO)
    return emscripten_get_device_pixel_ratio();

  return value;
}

void scene_renderer_init(SceneRenderer *renderer) {
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, renderer,
                                 false, scene_renderer_resize_callback);
}

void scene_renderer_close(const SceneRenderer *renderer) {
  wgpuRenderPipelineRelease(renderer->wgpu.pipeline);
  wgpuSwapChainRelease(renderer->draw.swapchain);
  wgpuQueueRelease(renderer->wgpu.queue);
  wgpuDeviceRelease(renderer->wgpu.device);
  wgpuInstanceRelease(renderer->wgpu.instance);
}

/**
   Initialize scene renderer main render pass and define their configurations.
 */
void scene_renderer_init_render_pass(
    SceneRenderer *renderer, const PipelineMultisampleCount multisample) {

  // init Scene render pass
  render_pass_create(&renderer->draw.pass[RenderPassType_Scene],
                     &(RenderPassCreateDescriptor){
                         .label = "Scene Render Pass",
                         .multisample = multisample,
                         .swapchain = &renderer->draw.swapchain,
                         .width = scene_renderer_width(renderer),
                         .height = scene_renderer_height(renderer),
                         .device = scene_renderer_device(renderer),
                         .queue = scene_renderer_queue(renderer),
                         .color =
                             {
                                 .view = NULL,
                                 .clear_value = renderer->background,
                                 .load_op = WGPULoadOp_Clear,
                                 .store_op = WGPUStoreOp_Store,
                                 .depth_slice = WGPU_DEPTH_SLICE_UNDEFINED,
                             },
                         .depth =
                             {
                                 .view = NULL,
                                 // Allow depth write
                                 .read_only = false,
                                 // Far plane
                                 .clear_value = 1.0f,
                                 // Keep depth for later use
                                 .store_op = WGPUStoreOp_Store,
                                 // Clear depth at start of render pass
                                 .load_op = WGPULoadOp_Clear,
                             },
                     });

  // init Gizmo render pass

  // create dedicated depth texture for gizmo
  render_pass_create(&renderer->draw.pass[RenderPassType_Gizmo],
                     &(RenderPassCreateDescriptor){
                         .label = "Gizmo Render Pass",
                         .multisample = multisample,
                         .swapchain = &renderer->draw.swapchain,
                         .width = scene_renderer_width(renderer),
                         .height = scene_renderer_height(renderer),
                         .device = scene_renderer_device(renderer),
                         .queue = scene_renderer_queue(renderer),
                         .color =
                             {
                                 .view = NULL,
                                 .clear_value = 0,
                                 .load_op = WGPULoadOp_Load,
                                 .store_op = WGPUStoreOp_Store,
                                 .depth_slice = WGPU_DEPTH_SLICE_UNDEFINED,
                             },
                         .depth =
                             {
                                 .view = NULL,
                                 .read_only = false,
                                 .clear_value = 1.0f,
                                 // clear previously rendered depth
                                 .load_op = WGPULoadOp_Clear,
                                 // do not store it afterward
                                 .store_op = WGPUStoreOp_Discard,
                             },
                     });
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
                                      void *data) {

  // do not add if max hook reached
  if (renderer->draw.callbacks.length == SCENE_RENDERER_MAX_HOOK) {
    VERBOSE_WARNING("Max hook reached.\n");
    return;
  }

  // add hook
  renderer->draw.callbacks.entries[renderer->draw.callbacks.length++] =
      (SceneRendererDrawCallback){
          .callback = callback,
          .data = data,
      };
}

void scene_renderer_render(void *desc) {

  SceneRendererRenderDescriptor *config = (SceneRendererRenderDescriptor *)desc;

  // Call draw callbacks
  for (size_t i = 0; i < config->renderer->draw.callbacks.length; i++) {
    SceneRendererDrawCallback *cb =
        &config->renderer->draw.callbacks.entries[i];

    // call callback, pass renderer and data
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
WGPUDevice scene_renderer_device(SceneRenderer *rd) { return rd->wgpu.device; }
WGPUQueue scene_renderer_queue(SceneRenderer *rd) { return rd->wgpu.queue; }

int scene_renderer_width(const SceneRenderer *rd) { return rd->context.width; }
int scene_renderer_height(const SceneRenderer *rd) {
  return rd->context.height;
}

const char *scene_renderer_target(SceneRenderer *rd) {
  return rd->context.name;
}

void scene_renderer_set_draw_mode(SceneRenderer *renderer,
                                  const SceneRendererDrawMode mode) {
  renderer->draw.mode = mode;
}

RenderPass *scene_renderer_pass(SceneRenderer *renderer,
                                const RenderPassType render_pass) {
  return &renderer->draw.pass[render_pass];
}

const SceneRendererDrawMode scene_renderer_draw_mode(SceneRenderer *renderer) {
  return renderer->draw.mode;
}

cclock *scene_renderer_clock(SceneRenderer *renderer) {
  return &renderer->clock;
}

WGPUSwapChain scene_renderer_create_swapchain(const SceneRenderer *renderer) {
  WGPUSurface surface = wgpuInstanceCreateSurface(
      renderer->wgpu.instance,
      &(WGPUSurfaceDescriptor){
          .nextInChain = (WGPUChainedStruct *)(&(
              WGPUSurfaceDescriptorFromCanvasHTMLSelector){
              .chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
              .selector = renderer->context.name,
          }),
      });

  return wgpuDeviceCreateSwapChain(
      renderer->wgpu.device, surface,
      &(WGPUSwapChainDescriptor){
          .usage = WGPUTextureUsage_RenderAttachment,
          .format = WGPUTextureFormat_BGRA8Unorm,
          .width = scene_renderer_width(renderer),
          .height = scene_renderer_height(renderer),
          .presentMode = WGPUPresentMode_Fifo,
      });
}
