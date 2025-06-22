#include "renderer.h"
#include "ao_bake.h"
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include "shadow_pass.h"
#include "webgpu/webgpu.h"

static int renderer_resize(Renderer *, int, const EmscriptenUiEvent *, void *);
static WGPUSwapChain renderer_create_swapchain(const Renderer *);
static void renderer_create_texture_view(const Renderer *, WGPUTextureView *);
static void renderer_create_multisampling_view(Renderer *);
static void renderer_render(void *);
static double renderer_dpi(double);

static WGPURenderPassColorAttachment
renderer_color_attachment_multisample(Renderer *, WGPUTextureView);
static WGPURenderPassColorAttachment
renderer_color_attachment_monosample(Renderer *, WGPUTextureView);

/**
   Depending on mono sampling or multi sampling, the pass color attachment of
   renderer will be different (i.e. no resolveTarget for monosampling). As to
   avoid branching during the draw function, we set those callback as parameters
   before calling the draw.

   MSAA Texture (Nx) ===> resolved ===> Swapchain texture (1x)
 */
static WGPURenderPassColorAttachment
renderer_color_attachment_multisample(Renderer *renderer,
                                      WGPUTextureView swapchain_view) {
  return (WGPURenderPassColorAttachment){
      .view = renderer->multisampling.view, // pass 4x sample as view
      .resolveTarget = swapchain_view,      // 1x sampled
      .loadOp = WGPULoadOp_Clear,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = renderer->background,
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
  };
}

static WGPURenderPassColorAttachment
renderer_color_attachment_monosample(Renderer *renderer,
                                     WGPUTextureView swapchain_view) {
  return (WGPURenderPassColorAttachment){
      .view = swapchain_view, // 1x sampled
      .loadOp = WGPULoadOp_Clear,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = renderer->background,
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
  };
}

void renderer_create(Renderer *renderer, const RendererCreateDescriptor *rd) {

  renderer->context.name = rd->name;
  renderer->clock = rd->clock;
  renderer->background = rd->background;
  renderer->context.dpi = renderer_dpi(rd->dpi);

  // set wgpu data
  renderer->wgpu.instance = wgpuCreateInstance(NULL);
  renderer->wgpu.device = emscripten_webgpu_get_device();
  renderer->wgpu.queue = wgpuDeviceGetQueue(renderer->wgpu.device);

  // define context size
  renderer_resize(renderer, 0, NULL, NULL);

  // set multisampling
  renderer->multisampling.count = rd->multisampling_count;
  renderer->multisampling.view = NULL;
  if (renderer->multisampling.count > 1)
    renderer_create_multisampling_view(renderer);
}

int renderer_resize(Renderer *renderer, int event_type,
                    const EmscriptenUiEvent *ui_event, void *user_data) {

  double w, h;

  // retrieve canvas dimension
  emscripten_get_element_css_size(renderer->context.name, &w, &h);

  // define render resolution
  renderer->context.width = (int)w * renderer->context.dpi;
  renderer->context.height = (int)h * renderer->context.dpi;

  // set canvas size
  emscripten_set_element_css_size(renderer->context.name, w, h);

  // reset swap chain on resize
  if (renderer->wgpu.swapchain) {
    wgpuSwapChainRelease(renderer->wgpu.swapchain);
    renderer->wgpu.swapchain = NULL;
  }

  renderer->wgpu.swapchain = renderer_create_swapchain(renderer);

  return 1;
}

static double renderer_dpi(double value) {

  // request dpi
  if (value == RENDERER_DPI_AUTO)
    return emscripten_get_device_pixel_ratio();

  return value;
}

void renderer_init(Renderer *renderer) {
  renderer_resize(renderer, 0, NULL, NULL);
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, false,
                                 (em_ui_callback_func)renderer_resize);
}

WGPUSwapChain renderer_create_swapchain(const Renderer *renderer) {
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

void renderer_close(const Renderer *renderer) {
  wgpuRenderPipelineRelease(renderer->wgpu.pipeline);
  wgpuSwapChainRelease(renderer->wgpu.swapchain);
  wgpuQueueRelease(renderer->wgpu.queue);
  wgpuDeviceRelease(renderer->wgpu.device);
  wgpuInstanceRelease(renderer->wgpu.instance);
}

void renderer_create_texture_view(const Renderer *renderer,
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
          .sampleCount = renderer->multisampling.count,
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

/**
   Create the texture and texture view for the multisampling rendering
 */
void renderer_create_multisampling_view(Renderer *renderer) {

  WGPUTexture msaa_texture = wgpuDeviceCreateTexture(
      renderer->wgpu.device,
      &(WGPUTextureDescriptor){
          .usage = WGPUTextureUsage_RenderAttachment,
          .size =
              (WGPUExtent3D){
                  .width = renderer->context.width,
                  .height = renderer->context.height,
                  .depthOrArrayLayers = 1,
              },
          .format = WGPUTextureFormat_BGRA8Unorm, // swapchain format
          .sampleCount = renderer->multisampling.count,
          .mipLevelCount = 1,
      });

  renderer->multisampling.view = wgpuTextureCreateView(msaa_texture, NULL);
}

void renderer_render(void *desc) {

  RendererRenderDescriptor *config = (RendererRenderDescriptor *)desc;

  // create swapchain texture view (1x sampled)
  WGPUTextureView swapchain_view =
      wgpuSwapChainGetCurrentTextureView(config->renderer->wgpu.swapchain);

  /* Create command encoder
     NOTE: Encoder records GPU operations such as:
     - Texture upload
     - Buffer upload
     - Render passes
     - Compute passes
  */

  WGPUCommandEncoder render_encoder =
      wgpuDeviceCreateCommandEncoder(config->renderer->wgpu.device, NULL);

  WGPUTextureView depth_texture_view;
  renderer_create_texture_view(config->renderer, &depth_texture_view);

  WGPURenderPassColorAttachment color_attachments =
      config->color_attachment_callback(config->renderer, swapchain_view);

  // attach depth texture to render pass to WGPU knows where to
  // write depth values
  WGPURenderPassDepthStencilAttachment depth_attachments =
      (WGPURenderPassDepthStencilAttachment){
          .view = depth_texture_view,
          .depthClearValue = 1.0f, // far plane
          .depthLoadOp =
              WGPULoadOp_Clear, // Clear depth at start of render pass
          .depthStoreOp = WGPUStoreOp_Store, // Keep depth for later use
          .depthReadOnly = false,            // Allow depth write
      };

  // begin render pass
  WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(
      render_encoder, &(WGPURenderPassDescriptor){
                          .label = "Final Render Pass",
                          // color attachments
                          .colorAttachmentCount = 1,
                          .colorAttachments = &color_attachments,
                          .depthStencilAttachment = &depth_attachments,
                      });

  // draw dynamic mesh
  config->draw_callback(config->scene, &render_pass);

  // draw fixed mesh
  scene_draw_fixed(config->scene, &render_pass);

  // end render pass
  wgpuRenderPassEncoderEnd(render_pass);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(config->renderer->wgpu.queue, 1, &render_buffer);

  // release all
  wgpuRenderPassEncoderRelease(render_pass);
  wgpuCommandEncoderRelease(render_encoder);
  wgpuCommandBufferRelease(render_buffer);
  wgpuTextureViewRelease(swapchain_view);

  // update clock delta
  clock_update_delta(config->renderer->clock);
}

/**
   Draw a scene with a specified draw mode along with the render pass that comes
   with it (ao, shadow mapping...). Also call the main loop.
 */
void renderer_draw(Renderer *renderer, Scene *scene,
                   const RendererDrawMode draw_mode) {

  PipelineMultisampleCount sample_count = renderer->multisampling.count;

  // Fixed (static) rendering
  scene_build_fixed(scene, sample_count); // build fixed (by default)

  // Dynamic rendering
  scene_draw_callback draw_callback;

  switch (draw_mode) {

  case RendererDrawMode_Solid:
    scene_build_solid(scene, sample_count); // build solid
    draw_callback = scene_draw_solid;       // draw solid callback
    break;

  case RendererDrawMode_Wireframe:
    scene_build_wireframe(scene, sample_count); // build wireframe
    draw_callback = scene_draw_wireframe;       // draw wireframe callback
    break;

  case RendererDrawMode_Texture:

    // Bake AO textures for static scenes elements
    renderer_bake_ao(renderer, scene);

    // Setup drawing pass may need to move it else where
    renderer_compute_shadow(renderer, scene);

    scene_build_texture(scene, sample_count); // build texture
    draw_callback = scene_draw_texture;       // build wireframe callback
    break;
  }

  /* Define render color attachment callback based on multisample count.
     Using callback prevents branching within the main loop
   */
  renderer_color_attachment_callback color_cbk =
      renderer->multisampling.count > 1 ? renderer_color_attachment_multisample
                                        : renderer_color_attachment_monosample;

  // call main loop
  emscripten_set_main_loop_arg(renderer_render,
                               &(RendererRenderDescriptor){
                                   .renderer = renderer,
                                   .scene = scene,
                                   .draw_callback = draw_callback,
                                   .color_attachment_callback = color_cbk,
                               },
                               0, 1);
}

void renderer_bake_ao(Renderer *renderer, Scene *scene) {

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
void renderer_compute_shadow(Renderer *renderer, Scene *scene) {
  shadow_pass_init(scene, renderer->wgpu.device, renderer->wgpu.queue);
}

WGPUDevice *renderer_device(Renderer *rd) { return &rd->wgpu.device; }
WGPUQueue *renderer_queue(Renderer *rd) { return &rd->wgpu.queue; }

int renderer_width(Renderer *rd) { return rd->context.width; }
int renderer_height(Renderer *rd) { return rd->context.height; }
