#include "renderer.h"
#include "../runtime/material.h"
#include "../utils/system.h"
#include "ao_bake.h"
#include "clock.h"
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include "shadow_pass.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>

static int renderer_resize(renderer *, int, const EmscriptenUiEvent *, void *);
static WGPUSwapChain renderer_create_swapchain(const renderer *);
static void renderer_create_texture_view(const renderer *, WGPUTextureView *);

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

void renderer_create_texture_view(const renderer *renderer,
                                         WGPUTextureView *texture_view) {

  // Need to create a texture view for Z buffer stencil
  // by default set depth based on draw call order (first ones in
  // backgrounds...)
  // => Need to create a depth texture: a hidden buffer storing depth values for
  // each pixel
  WGPUTexture depthTexture = wgpuDeviceCreateTexture(
      renderer->wgpu.device,
      &(WGPUTextureDescriptor){
          .usage = WGPUTextureUsage_RenderAttachment, // used in rendering pass
          .size =
              (WGPUExtent3D){
                  .width = renderer->context.width,
                  .height = renderer->context.height,
                  .depthOrArrayLayers = 1,
              },
          .format =
              WGPUTextureFormat_Depth24Plus, // texture with 24bit-depth format
          .mipLevelCount = 1,
          .sampleCount = 1,
          .dimension = WGPUTextureDimension_2D,
      });

  *texture_view = wgpuTextureCreateView(
      depthTexture,
      &(WGPUTextureViewDescriptor){
          .format = WGPUTextureFormat_Depth24Plus,
          .dimension = WGPUTextureViewDimension_2D,
          .baseMipLevel = 0,
          .mipLevelCount = 1, // match above texture
          .baseArrayLayer = 0,
          .arrayLayerCount = 1, // not using array texture (only 1)
          .aspect = WGPUTextureAspect_DepthOnly,
      });
}

void renderer_draw(const renderer *renderer, scene *scene) {

  // create texture view
  WGPUTextureView back_buffer =
      wgpuSwapChainGetCurrentTextureView(renderer->wgpu.swapchain);

  // create command encoder
  // encoder records GPU operations:
  // - Texture upload
  // - Buffer upload
  // - Render passes
  // - Compute passes

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(renderer->wgpu.device, NULL);

  WGPUTextureView depth_texture_view;
  renderer_create_texture_view(renderer, &depth_texture_view);

  // begin render pass
  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(
      render_encoder,
      &(WGPURenderPassDescriptor){
          .label = "Texture Render Pass",
          // color attachments
          .colorAttachmentCount = 1,
          .colorAttachments =
              &(WGPURenderPassColorAttachment){
                  .view = back_buffer,
                  .loadOp = WGPULoadOp_Clear,
                  .storeOp = WGPUStoreOp_Store,
                  .clearValue = (WGPUColor){0.15f, 0.15f, 0.18f, 1.0f},
                  .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
              },
          .depthStencilAttachment = &(WGPURenderPassDepthStencilAttachment){
              // attach depth texture to render pass to WGPU knows where to
              // write depth values
              .view = depth_texture_view,
              .depthClearValue = 1.0f, // far plane
              .depthLoadOp =
                  WGPULoadOp_Clear, // Clear depth at start of render pass
              .depthStoreOp = WGPUStoreOp_Store, // Keep depth for later use
              .depthReadOnly = false,            // Allow depth write
          }});

  // draw mesh scene
  scene_draw(scene, MESH_SHADER_DEFAULT, &render_pass);

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

void renderer_bake_ao(renderer *renderer, scene *scene) {

  ao_bake_init(&(AOBakeInitDescriptor){
      .mesh_list = &scene->layer.lit,
      .scene = scene,
      .queue = &renderer->wgpu.queue,
      .device = &renderer->wgpu.device,
  });
}

/**
   Main entry point of the shadow computing pass
 */
void renderer_compute_shadow(renderer *renderer, scene *scene) {
  shadow_pass_init(scene, renderer->wgpu.device, renderer->wgpu.queue);
}
