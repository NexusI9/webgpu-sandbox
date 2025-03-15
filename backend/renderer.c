#include "renderer.h"
#include "clock.h"
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include "webgpu/webgpu.h"

static int renderer_resize(renderer *, int, const EmscriptenUiEvent *, void *);
static WGPUSwapChain renderer_create_swapchain(const renderer *);

renderer renderer_create(const RendererCreateDescriptor *rd) {

  renderer new_renderer;
  new_renderer.context.name = rd->name;
  new_renderer.clock = rd->clock;
  new_renderer.wgpu.instance = wgpuCreateInstance(NULL);
  new_renderer.wgpu.device = emscripten_webgpu_get_device();
  new_renderer.wgpu.queue = wgpuDeviceGetQueue(new_renderer.wgpu.device);

  if (rd->lock_mouse)
    renderer_lock_mouse(&new_renderer);

  return new_renderer;
}

int renderer_resize(renderer *renderer, int event_type,
                    const EmscriptenUiEvent *ui_event, void *user_data) {

  double w, h;

  // retrieve canvas dimension
  emscripten_get_element_css_size(renderer->context.name, &w, &h);
  renderer->context.width = (int)w;
  renderer->context.height = (int)h;

  // set canvas size
  emscripten_set_element_css_size(renderer->context.name,
                                  renderer->context.width,
                                  renderer->context.height);

  // reset swap chain on resize
  if (renderer->wgpu.swapchain) {
    wgpuSwapChainRelease(renderer->wgpu.swapchain);
    renderer->wgpu.swapchain = NULL;
  }

  renderer->wgpu.swapchain = renderer_create_swapchain(renderer);

  return 1;
}

void renderer_init(renderer *renderer) {
  renderer_resize(renderer, 0, NULL, NULL);
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, false,
                                 (em_ui_callback_func)renderer_resize);
}

WGPUSwapChain renderer_create_swapchain(const renderer *renderer) {
  WGPUSurface surface = wgpuInstanceCreateSurface(
      renderer->wgpu.instance,
      &(WGPUSurfaceDescriptor){
          .nextInChain = (WGPUChainedStruct *)(&(
              WGPUSurfaceDescriptorFromCanvasHTMLSelector){
              .chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector,
              .selector = renderer->context.name,
          })});

  return wgpuDeviceCreateSwapChain(
      renderer->wgpu.device, surface,
      &(WGPUSwapChainDescriptor){
          .usage = WGPUTextureUsage_RenderAttachment,
          .format = WGPUTextureFormat_BGRA8Unorm,
          .width = renderer->context.width,
          .height = renderer->context.height,
          .presentMode = WGPUPresentMode_Fifo,
      });
}

void renderer_end_frame(const renderer *renderer) {

  wgpuRenderPipelineRelease(renderer->wgpu.pipeline);
  wgpuSwapChainRelease(renderer->wgpu.swapchain);
  wgpuQueueRelease(renderer->wgpu.queue);
  wgpuDeviceRelease(renderer->wgpu.device);
  wgpuInstanceRelease(renderer->wgpu.instance);
}

void renderer_draw(const renderer *renderer, scene *scene) {

  // create texture view
  WGPUTextureView back_buffer =
      wgpuSwapChainGetCurrentTextureView(renderer->wgpu.swapchain);

  // create command encoder
  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(renderer->wgpu.device, NULL);

  // begin render pass
  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(
      render_encoder,
      &(WGPURenderPassDescriptor){
          // color attachments
          .colorAttachmentCount = 1,
          .colorAttachments =
              &(WGPURenderPassColorAttachment){
                  .view = back_buffer,
                  .loadOp = WGPULoadOp_Clear,
                  .storeOp = WGPUStoreOp_Store,
                  .clearValue = (WGPUColor){0.15f, 0.15f, 0.18f, 1.0f},
                  .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED},
      });

  // draw mesh scene
  scene_draw(scene, &render_pass);

  // end render pass
  wgpuRenderPassEncoderEnd(render_pass);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(renderer->wgpu.queue, 1, &render_buffer);

  // release all
  wgpuRenderPassEncoderRelease(render_pass);
  wgpuCommandEncoderRelease(render_encoder);
  wgpuCommandBufferRelease(render_buffer);
  wgpuTextureViewRelease(back_buffer);

  // update clock delta
  clock_update_delta(renderer->clock);
}

void renderer_set_draw(const void *callback) {
  emscripten_set_main_loop(callback, 0, 1);
}

void renderer_lock_mouse(const renderer *renderer) {
  emscripten_request_pointerlock(renderer->context.name, true);
}
