#include "core.h"
#include "../runtime/html_event/html_event.h"
#include "../runtime/input/input.h"
#include "../utils/system.h"
#include "ao_bake.h"
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include "shadow_pass.h"
#include "webgpu/webgpu.h"
#include <string.h>

static void scene_renderer_init(SceneRenderer *);
static int scene_renderer_resize(SceneRenderer *, int,
                                 const EmscriptenUiEvent *, void *);
static WGPUSwapChain scene_renderer_create_swapchain(const SceneRenderer *);
static void scene_renderer_create_texture_view(const SceneRenderer *,
                                               WGPUTextureView *);
static void scene_renderer_create_multisampling_view(SceneRenderer *);
static void scene_renderer_render(void *);
static double scene_renderer_dpi(double);

static WGPURenderPassColorAttachment
scene_renderer_color_attachment_multisample(SceneRenderer *, WGPUTextureView);
static WGPURenderPassColorAttachment
scene_renderer_color_attachment_monosample(SceneRenderer *, WGPUTextureView);

/**
   Depending on mono sampling or multi sampling, the pass color attachment of
   renderer will be different (i.e. no resolveTarget for monosampling). As to
   avoid branching during the draw function, we set those callback as parameters
   before calling the draw.

   MSAA Texture (Nx) ===> resolved ===> Swapchain texture (1x)
 */
WGPURenderPassColorAttachment
scene_renderer_color_attachment_multisample(SceneRenderer *renderer,
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

WGPURenderPassColorAttachment
scene_renderer_color_attachment_monosample(SceneRenderer *renderer,
                                           WGPUTextureView swapchain_view) {
  return (WGPURenderPassColorAttachment){
      .view = swapchain_view, // 1x sampled
      .loadOp = WGPULoadOp_Clear,
      .storeOp = WGPUStoreOp_Store,
      .clearValue = renderer->background,
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
  };
}

void scene_renderer_create(SceneRenderer *renderer,
                           const SceneRendererCreateDescriptor *rd) {

  renderer->context.name = rd->name;
  renderer->clock = rd->clock;
  renderer->background = rd->background;
  renderer->context.dpi = scene_renderer_dpi(rd->dpi);

  // set wgpu data
  renderer->wgpu.instance = wgpuCreateInstance(NULL);
  renderer->wgpu.device = emscripten_webgpu_get_device();
  renderer->wgpu.queue = wgpuDeviceGetQueue(renderer->wgpu.device);

  // define context size
  scene_renderer_resize(renderer, 0, NULL, NULL);

  // set multisampling
  renderer->multisampling.count = rd->multisampling_count;
  renderer->multisampling.view = NULL;
  if (renderer->multisampling.count > 1)
    scene_renderer_create_multisampling_view(renderer);

  // set depth texture view
  scene_renderer_create_texture_view(renderer, &renderer->depth.view);

  // Global Input & Event polling

  // init global HTML event manager with context name (implicit)
  html_event_init(rd->name);

  // poll global input
  input_listen();

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
   1 - Solid config
   2 - Wireframe config
   3 - Boundbox config

   By following this order, we can simply map the right array entry depending on
   the scene render mode.
 */
void scene_renderer_set_draw_layout(SceneRenderer *renderer,
                                    const SceneRendererDrawMode mode,
                                    const SceneRendererDrawLayoutList *layout) {

  if (mode >= SCENE_RENDERER_DRAW_MODE_COUNT)
    return;

  // assign values
  renderer->draw_layouts[mode] = (SceneRendererDrawLayoutList){
      .length = layout->length,
  };

  // copy mesh ref lists
  memcpy(renderer->draw_layouts[mode].entries, layout->entries,
         sizeof(SceneRendererDrawLayout) * layout->length);
}

int scene_renderer_resize(SceneRenderer *renderer, int event_type,
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

  renderer->wgpu.swapchain = scene_renderer_create_swapchain(renderer);

  return 1;
}

static double scene_renderer_dpi(double value) {

  // request dpi
  if (value == SCENE_RENDERER_DPI_AUTO)
    return emscripten_get_device_pixel_ratio();

  return value;
}

void scene_renderer_init(SceneRenderer *renderer) {
  scene_renderer_resize(renderer, 0, NULL, NULL);
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, false,
                                 (em_ui_callback_func)scene_renderer_resize);
}

WGPUSwapChain scene_renderer_create_swapchain(const SceneRenderer *renderer) {
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

void scene_renderer_close(const SceneRenderer *renderer) {
  wgpuRenderPipelineRelease(renderer->wgpu.pipeline);
  wgpuSwapChainRelease(renderer->wgpu.swapchain);
  wgpuQueueRelease(renderer->wgpu.queue);
  wgpuDeviceRelease(renderer->wgpu.device);
  wgpuInstanceRelease(renderer->wgpu.instance);
}

void scene_renderer_create_texture_view(const SceneRenderer *renderer,
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
void scene_renderer_create_multisampling_view(SceneRenderer *renderer) {

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
  if (renderer->draw_callbacks.length == SCENE_RENDERER_MAX_HOOK) {
    VERBOSE_WARNING("Max hook reached.\n");
    return;
  }

  // add hook
  renderer->draw_callbacks.entries[renderer->draw_callbacks.length++] =
      (SceneRendererDrawCallback){
          .callback = callback,
          .data = data,
      };
}

void scene_renderer_render(void *desc) {

  SceneRendererRenderDescriptor *config = (SceneRendererRenderDescriptor *)desc;

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

  WGPURenderPassColorAttachment color_attachments =
      config->color_attachment_callback(config->renderer, swapchain_view);

  // attach depth texture to render pass to WGPU knows where to
  // write depth values
  WGPURenderPassDepthStencilAttachment depth_attachments =
      (WGPURenderPassDepthStencilAttachment){
          .view = config->renderer->depth.view,
          .depthClearValue = 1.0f, // far plane
          .depthLoadOp =
              WGPULoadOp_Clear, // Clear depth at start of render pass
          .depthStoreOp = WGPUStoreOp_Store, // Keep depth for later use
          .depthReadOnly = false,            // Allow depth write
      };

  // begin render pass
  config->renderer->wgpu.render_pass = wgpuCommandEncoderBeginRenderPass(
      render_encoder, &(WGPURenderPassDescriptor){
                          .label = "Final Render Pass",
                          // color attachments
                          .colorAttachmentCount = 1,
                          .colorAttachments = &color_attachments,
                          .depthStencilAttachment = &depth_attachments,
                      });

  // Call draw callbacks
  for (size_t i = 0; i < config->renderer->draw_callbacks.length; i++) {
    SceneRendererDrawCallback *cb =
        &config->renderer->draw_callbacks.entries[i];

    // call callback, pass renderer and data
    cb->callback(cb->data);
  }

  // end render pass
  wgpuRenderPassEncoderEnd(config->renderer->wgpu.render_pass);

  // create command buffer
  WGPUCommandBuffer render_buffer =
      wgpuCommandEncoderFinish(render_encoder, NULL); // after 'end render pass'

  // submit commands
  wgpuQueueSubmit(config->renderer->wgpu.queue, 1, &render_buffer);

  // release all
  wgpuRenderPassEncoderRelease(config->renderer->wgpu.render_pass);
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
void scene_renderer_draw(SceneRenderer *renderer) {

  PipelineMultisampleCount sample_count = renderer->multisampling.count;

  /* Define render color attachment callback based on multisample count.
     Using callback prevents branching within the main loop
   */
  scene_renderer_color_attachment_callback color_cbk =
      renderer->multisampling.count > 1
          ? scene_renderer_color_attachment_multisample
          : scene_renderer_color_attachment_monosample;

  // call main loop
  emscripten_set_main_loop_arg(scene_renderer_render,
                               &(SceneRendererRenderDescriptor){
                                   .renderer = renderer,
                                   .color_attachment_callback = color_cbk,
                               },
                               0, 1);
}

// getters
WGPUDevice *scene_renderer_device(SceneRenderer *rd) {
  return &rd->wgpu.device;
}
WGPUQueue *scene_renderer_queue(SceneRenderer *rd) { return &rd->wgpu.queue; }

int scene_renderer_width(SceneRenderer *rd) { return rd->context.width; }
int scene_renderer_height(SceneRenderer *rd) { return rd->context.height; }

const char *scene_renderer_target(SceneRenderer *rd) {
  return rd->context.name;
}

void scene_renderer_set_draw_mode(SceneRenderer *renderer,
                                  const SceneRendererDrawMode mode) {
  renderer->draw_mode = mode;
}
