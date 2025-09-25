#include "context.h"
#include "backend/logger.h"
#include "backend/std_pipeline/core.h"
#include "backend/std_texture/core.h"
#include "emscripten/em_types.h"
#include "emscripten/emscripten.h"
#include "emscripten/html5_webgpu.h"
#include "runtime/html_event/core.h"
#include "runtime/texture/core.h"
#include "string.h"

Context g_context = {0};

static inline WGPUSwapChain context_create_swapchain();

static bool context_update_size(int event_type,
                                const EmscriptenUiEvent *ui_event,
                                void *user_data);

ContextStatus context_init(const ContextDescriptor *desc) {

  g_context.html_target = strdup(desc->html_target);
  g_context.instance = wgpuCreateInstance(NULL);
  g_context.device = emscripten_webgpu_get_device();
  g_context.queue = wgpuDeviceGetQueue(g_context.device);
  g_context.dpi = emscripten_get_device_pixel_ratio();
  g_context.multisample = desc->render.multisample_count;

  context_update_size(0, NULL, (void *)&g_context);

  // === Global Input & Event polling ===

  TIMER("Fallback Textures", { standard_textures_init(); });

  TIMER("Standard Shaders", {
    standard_render_pipelines_init(desc->render.multisample_count);
    standard_compute_pipelines_init();
  });

  // init global HTML event manager
  html_event_init(g_context.html_target);

  // init resize event
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,
                                 (void *)&g_context, false,
                                 context_update_size);

  // poll global input
  input_init(desc->input);

  printf("context width: %d\n", context_width());
  printf("context height: %d\n", context_height());

  return ContextStatus_Success;
}

WGPUSwapChain context_create_swapchain() {
  WGPUSurface surface = wgpuInstanceCreateSurface(
      g_context.instance,
      &(WGPUSurfaceDescriptor){
          .nextInChain = (WGPUChainedStruct *)(&(
              WGPUSurfaceDescriptorFromCanvasHTMLSelector){
              .chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
              .selector = g_context.html_target,
          }),
      });

  return wgpuDeviceCreateSwapChain(
      g_context.device, surface,
      &(WGPUSwapChainDescriptor){
          .usage = WGPUTextureUsage_RenderAttachment,
          .format = TEXTURE_FORMAT_ONSCREEN_DEFAULT,
          .width = g_context.width * g_context.dpi,
          .height = g_context.height * g_context.dpi,
          .presentMode = WGPUPresentMode_Fifo,
      });
}

bool context_update_size(int event_type, const EmscriptenUiEvent *ui_event,
                         void *user_data) {

  Context *context = (Context *)user_data;
  double w, h;

  // retrieve canvas dimension
  emscripten_get_element_css_size(context->html_target, &w, &h);

  // define render resolution
  context->width = (int)w;
  context->height = (int)h;

  // set canvas size
  emscripten_set_element_css_size(context->html_target, w, h);

  if (context->swapchain) {
    // wgpuSwapChainRelease(renderer->wgpu.swapchain);
    // renderer->wgpu.swapchain = NULL;
  }

  context->swapchain = context_create_swapchain();

  return EM_FALSE;
}

ContextStatus context_close() {

  wgpuSwapChainRelease(g_context.swapchain);
  wgpuQueueRelease(g_context.queue);
  wgpuDeviceRelease(g_context.device);
  wgpuInstanceRelease(g_context.instance);

  return ContextStatus_Success;
}
