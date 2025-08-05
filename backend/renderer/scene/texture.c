#include "texture.h"
#include "../backend/buffer.h"
#include "../utils/system.h"
#include "./shadow_map/shadow_map.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

// black color
static const uint8_t pixel[4] = {255, 255, 255, 255};

/* DELETE ME ?
static inline void
scene_renderer_create_fallback_texture_2d(Texture *, const WGPUDevice, const
WGPUQueue);

void scene_renderer_create_fallback_texture_2d(Texture *texture,
                                               const WGPUDevice device,
                                               const WGPUQueue queue) {
  texture_create(texture, &(TextureCreateDescriptor){
                              .channels = 4,
                              .width = 1,
                              .height = 1,
                              .value = (uint8_t[]){0, 0, 0, 255},
                          });
}
 */

static inline void scene_renderer_create_fallback_texture_2d(WGPUTexture *,
                                                             const WGPUDevice,
                                                             const WGPUQueue);
static inline void
scene_renderer_create_fallback_depth_cube_array(WGPUTexture *, const WGPUDevice,
                                                const WGPUQueue);

static inline void
scene_renderer_create_fallback_depth_2d_array(WGPUTexture *, const WGPUDevice,
                                              const WGPUQueue);

/**
   ▗▄▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖
   ▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘
   ▐▛▀▀▘▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖
   ▐▌   ▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌

   Create and store the basic fallback texture set as placeholder in more
   complex shader like PBR before being replace by computational intensive
   process like shadow map or AO baking.
 */
void scene_renderer_init_fallback_textures(SceneRenderer *renderer) {

  VERBOSE_PROCESS("Caching fallback textures...");

  const WGPUDevice device = renderer->wgpu.device;
  const WGPUQueue queue = renderer->wgpu.queue;

  // create texture 2D view fallback
  scene_renderer_create_fallback_texture_2d(
      &renderer->texture.fallback.texture_2d, device, queue);

  // create depth cube array view fallback
  scene_renderer_create_fallback_depth_cube_array(
      &renderer->texture.fallback.depth_cube_array, device, queue);

  // create depth 2d array view fallback
  scene_renderer_create_fallback_depth_2d_array(
      &renderer->texture.fallback.depth_2d_array, device, queue);
}

void scene_renderer_create_fallback_texture_2d(WGPUTexture *texture,
                                               const WGPUDevice device,
                                               const WGPUQueue queue) {

  *texture = wgpuDeviceCreateTexture(
      device,
      &(WGPUTextureDescriptor){
          .size =
              {
                  .width = 1,
                  .height = 1,
                  .depthOrArrayLayers = 1,
              },
          .format = WGPUTextureFormat_R8Unorm,
          .mipLevelCount = 1,
          .sampleCount = 1,
          .dimension = WGPUTextureDimension_2D,
          .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
      });

  wgpuQueueWriteTexture(queue,
                        &(WGPUImageCopyTexture){
                            .texture = *texture,
                            .mipLevel = 0,
                            .origin = {0, 0, 0},
                            .aspect = WGPUTextureAspect_All,
                        },
                        (uint8_t[]){255}, sizeof(uint32_t),
                        &(WGPUTextureDataLayout){
                            .offset = 0,
                            .bytesPerRow = 1,
                            .rowsPerImage = 1,
                        },
                        &(WGPUExtent3D){1, 1, 1});
}

void scene_renderer_create_fallback_depth_cube_array(WGPUTexture *texture,
                                                     const WGPUDevice device,
                                                     const WGPUQueue queue) {

  // create texture
  *texture = wgpuDeviceCreateTexture(
      device, &(WGPUTextureDescriptor){
                   .label = "Fallback texture cube array",
                   .size =
                       (WGPUExtent3D){
                           .width = 1,
                           .height = 1,
                           .depthOrArrayLayers = 6,
                       },
                   .format = SHADOW_DEPTH_FORMAT,
                   .usage = WGPUTextureUsage_RenderAttachment |
                            WGPUTextureUsage_TextureBinding,
                   .dimension = WGPUTextureDimension_2D,
                   .mipLevelCount = 1,
                   .sampleCount = 1,
               });
}

void scene_renderer_create_fallback_depth_2d_array(WGPUTexture *texture,
                                                   const WGPUDevice device,
                                                   const WGPUQueue queue) {

  // create texture
  *texture = wgpuDeviceCreateTexture(
      device,
      &(WGPUTextureDescriptor){
          .label = "Fallback depth 2d array",
          .size =
              (WGPUExtent3D){
                  .width = 1,
                  .height = 1,
                  .depthOrArrayLayers = 1, // may need to update
              },
          .format = SHADOW_DEPTH_FORMAT,
          .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
          .dimension = WGPUTextureDimension_2D,
          .mipLevelCount = 1,
          .sampleCount = 1,
      });
}

WGPUTextureView scene_renderer_fallback_texture_view_2d(SceneRenderer *rd) {
  return wgpuTextureCreateView(rd->texture.fallback.texture_2d, NULL);
}

WGPUTextureView
scene_renderer_fallback_texture_view_depth_cube_array(SceneRenderer *rd) {

  return wgpuTextureCreateView(
      rd->texture.fallback.depth_cube_array,
      &(WGPUTextureViewDescriptor){
          .label = "Falbback texture view cube array",
          .dimension = WGPUTextureViewDimension_CubeArray,
          .format = SHADOW_DEPTH_FORMAT,
          .baseMipLevel = 0,
          .mipLevelCount = 1,
          .baseArrayLayer = 0,
          .arrayLayerCount = 6,
          .aspect = WGPUTextureAspect_DepthOnly,
      });
}

WGPUTextureView
scene_renderer_fallback_texture_view_depth_2d_array(SceneRenderer *rd) {

  return wgpuTextureCreateView(
      rd->texture.fallback.depth_2d_array,
      &(WGPUTextureViewDescriptor){
          .label = "Fallback 2D array view",
          .dimension = WGPUTextureViewDimension_2DArray,
          .format = SHADOW_DEPTH_FORMAT,
          .baseMipLevel = 0,
          .mipLevelCount = 1,
          .baseArrayLayer = 0,
          .arrayLayerCount = 1, // or however many layers
          .aspect = WGPUTextureAspect_DepthOnly,
      });
}

/**
   ▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖▗▄▄▄ ▗▄▄▄▖▗▄▄▖
   ▐▌ ▐▌▐▌   ▐▛▚▖▐▌▐▌  █▐▌   ▐▌ ▐▌
   ▐▛▀▚▖▐▛▀▀▘▐▌ ▝▜▌▐▌  █▐▛▀▀▘▐▛▀▚▖
   ▐▌ ▐▌▐▙▄▄▖▐▌  ▐▌▐▙▄▄▀▐▙▄▄▖▐▌ ▐▌

   Create shared color and depth render texture view shared accross the
   different render passes.
 */
void scene_renderer_init_render_textures(SceneRenderer *renderer) {

  VERBOSE_PROCESS("Creating shared render textures...");

  SceneRendererTextureDescriptor render_texture_config = {
      .width = scene_renderer_width(renderer),
      .height = scene_renderer_height(renderer),
      .multisample = renderer->texture.multisample,
      .device = scene_renderer_device(renderer),
  };

  if (renderer->texture.multisample != PipelineMultisampleCount_1x)
    scene_renderer_create_multisampling_view(&renderer->texture.render.color,
                                             &render_texture_config);

  scene_renderer_create_depth_view(&renderer->texture.render.depth,
                                   &render_texture_config);
}

/**
   Create the texture and texture view for the multisampling rendering.
 */
void scene_renderer_create_multisampling_view(
    WGPUTextureView *view, const SceneRendererTextureDescriptor *desc) {

  WGPUTexture msaa_texture = wgpuDeviceCreateTexture(
      desc->device,
      &(WGPUTextureDescriptor){
          .label = "MSAA Texture",
          .usage = WGPUTextureUsage_RenderAttachment,
          .size =
              (WGPUExtent3D){
                  .width = desc->width,
                  .height = desc->height,
                  .depthOrArrayLayers = 1,
              },
          .format = WGPUTextureFormat_BGRA8Unorm, // swapchain format
          .sampleCount = desc->multisample,
          .mipLevelCount = 1,
      });

  *view = wgpuTextureCreateView(msaa_texture, NULL);
}

void scene_renderer_create_depth_view(
    WGPUTextureView *view, const SceneRendererTextureDescriptor *desc) {

  // Need to create a texture view for Z buffer stencil
  // by default set depth based on draw call order (first ones in
  // backgrounds...)
  // => Need to create a depth texture: a hidden buffer storing depth values for
  // each pixel
  WGPUTexture depthTexture = wgpuDeviceCreateTexture(
      desc->device,
      &(WGPUTextureDescriptor){
          .usage = WGPUTextureUsage_RenderAttachment, // used in rendering pass
          .size =
              (WGPUExtent3D){
                  .width = desc->width,
                  .height = desc->height,
                  .depthOrArrayLayers = 1,
              },
          .format =
              WGPUTextureFormat_Depth24Plus, // texture with 24bit-depth format
          .mipLevelCount = 1,
          .sampleCount = desc->multisample,
          .dimension = WGPUTextureDimension_2D,
      });

  *view = wgpuTextureCreateView(
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
