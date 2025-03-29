#include "renderer.h"
#include "clock.h"
#include "emscripten/html5.h"
#include "emscripten/html5_webgpu.h"
#include "webgpu/webgpu.h"
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

static void renderer_create_texture_view(const renderer *renderer,
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
                  renderer->context.width,
                  renderer->context.height,
                  1,
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

/**
   The building shadow phase is segmented in numerous steps:

   1. BINDING:
   For each scene point and directional lights, we bind the relative views to
   the mesh shadow shader view matrix uniform

   2. DRAWING:
   Once the view matrix is bound, the mesh is ready to drawn with its shadow
  shader. For each light views, each mesh will be renderder (under a certain
  point of view)

   3. STORING:
  The rendered mesh will be stored in a dedicated Texture that will be held in a
  arrayed texture in the Light list

  4. TRANSFERT
  Once the rendered textures are stored, they will be uploaded in the mesh
  "default shader" as a texture and sampler to be read and reused for calculate
  the shadow


  */

static void renderer_shadow_to_texture(scene *, WGPUTexture *, uint32_t,
                                       WGPUDevice *);

static void renderer_create_shadow_map(renderer *, scene *, WGPUTexture *);

void renderer_shadow_to_texture(scene *scene, WGPUTexture *texture,
                                uint32_t layer, WGPUDevice *device) {

  printf("rendering shadow to texture\n");

  /*  Create a new "nested" texture view for each layer that points back to the
     texture。Both "global Texture view" and "indexed Texture view" point toward
                               the same texture

                           +----------------------+
                           |      WGPUTexture     |
                           +----------------------+
                                  /        \
   +----------------------------------+   +-------------------------------+
   |    WGPUTextureView<all layers>   |   |    WGPUTextureView<layer N>   |
   +----------------------------------+   +-------------------------------+
     Final Texture with all layers         Target a specific texture layer

                   |                                     |
                   |                                     |
                   |		 	 +-------------------------------+
                   |			 |        1.Renderpass           |
                   |			 +-------------------------------+
                   |                         Render to specific layer
                   |
                   |
    +-------------------------------+
    |  2. Shader<texture 2D array>  |
    +-------------------------------+
        Bind the Array to shader

   */

  WGPUTextureView layer_view = wgpuTextureCreateView(
      *texture, &(WGPUTextureViewDescriptor){
                    .label = "shadow contextual texture view",
                    .format = WGPUTextureFormat_Depth32Float,
                    .dimension = WGPUTextureViewDimension_2D,
                    .baseArrayLayer = layer,
                    .arrayLayerCount = 1,
                    .mipLevelCount = 1,
                });

  // create render pass and render it to the nested layer
  WGPUCommandEncoder shadow_encoder =
      wgpuDeviceCreateCommandEncoder(*device, NULL);

  WGPURenderPassEncoder shadow_pass = wgpuCommandEncoderBeginRenderPass(
      shadow_encoder, &(WGPURenderPassDescriptor){
                          .colorAttachmentCount = 0,
                          .depthStencilAttachment =
                              &(WGPURenderPassDepthStencilAttachment){
                                  .view = layer_view,
                                  .depthClearValue = 1.0f,
                                  .depthLoadOp = WGPULoadOp_Clear,
                                  .depthStoreOp = WGPUStoreOp_Store,
                              },
                      });

  scene_draw(scene, MESH_SHADER_SHADOW, &shadow_pass);

  wgpuRenderPassEncoderEnd(shadow_pass);
}

/**
    Shadow map array Generation:
    - bind light view projection
    - render each mesh under lights POV
    - render to the shadow array
 */
void renderer_create_shadow_map(renderer *renderer, scene *scene,
                                WGPUTexture *texture) {
  for (size_t p = 0; p < scene->lights.point.length; p++) {
    // retrieve 6 views of point cube
    PointLightViews light_views = light_point_views(
        scene->lights.point.items[p].position, &scene->viewport);

    // render scene and store depth map for each view
    for (size_t v = 0; v < light_views.length; v++) {
      mat4 *current_view = &light_views.views[v];
      printf("view: %lu\n", v);

      // 1. Bind meshes
      for (int m = 0; m < scene->meshes.solid.length; m++) {
        mesh *current_mesh = &scene->meshes.solid.items[m];
        mesh_bind_shadow(current_mesh, current_view);
        mesh_build(current_mesh, MESH_SHADER_SHADOW);
      }

      // 2. Render scene (create shadow render pass to texture layer)
      renderer_shadow_to_texture(scene, texture, p * light_views.length + v,
                                 &renderer->wgpu.device);

      // 3. Clear meshes bind group
      for (int m = 0; m < scene->meshes.solid.length; m++) {
        mesh *current_mesh = &scene->meshes.solid.items[m];
        mesh_clear_bindings(current_mesh, MESH_SHADER_SHADOW);
      }
    }
  }
}

void renderer_compute_shadow(renderer *renderer, scene *scene) {

  printf("==== COMPUTING SHADOW ====\n");

  // create multi layered light texture (passed to the renderpass)
  size_t point_light_length = scene->lights.point.length;
  uint32_t layer_count = LIGHT_POINT_VIEWS * point_light_length;

  // create shadow map texture
  WGPUTexture shadow_texture = wgpuDeviceCreateTexture(
      renderer->wgpu.device,
      &(WGPUTextureDescriptor){
          .size = (WGPUExtent3D){SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1},
          .format = WGPUTextureFormat_Depth32Float,
          .usage = WGPUTextureUsage_RenderAttachment |
                   WGPUTextureUsage_TextureBinding,
          .dimension = WGPUTextureDimension_2D,
          .mipLevelCount = 1,
      });

  // populate scene point light texture_view
  scene->lights.point.shadow_texture = wgpuTextureCreateView(
      shadow_texture, &(WGPUTextureViewDescriptor){
                          .label = "shadow global texture view",
                          .format = WGPUTextureFormat_Depth32Float,
                          .dimension = WGPUTextureViewDimension_2DArray,
                          .mipLevelCount = 1,
                          .arrayLayerCount = layer_count,
                          .baseArrayLayer = 0,
                          .aspect = WGPUTextureAspect_DepthOnly,
                      });

  // Generate Shadow maps
  renderer_create_shadow_map(renderer, scene, &shadow_texture);

  // 4. Transfer depth texture array to default shader
  for (int m = 0; m < scene->meshes.solid.length; m++) {
    mesh *current_mesh = &scene->meshes.solid.items[m];
    /*shader_add_texture_view(
        mesh_shader_default(current_mesh),
        &(ShaderCreateTextureViewDescriptor){
            .visibility = WGPUShaderStage_Vertex,
            .entry_count = 1,
            .group_index = 4,
            .entries = (ShaderBindGroupTextureViewEntry[]){{
                .binding = 0,
                .texture_view = scene->lights.point.shadow_texture,
            }},
        });*/
  }
}
