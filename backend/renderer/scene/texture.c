#include "texture.h"
#include "../backend/buffer.h"
#include "../utils/system.h"
#include "./shadow_pass.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

// black color
static const uint8_t pixel[4] = {255, 255, 255, 255};

static inline void
scene_renderer_create_fallback_texture_2d(Texture *, WGPUDevice *, WGPUQueue *);

static inline void
scene_renderer_create_fallback_texture_2d_view(WGPUTextureView *, WGPUDevice *,
                                               WGPUQueue *);
static inline void
scene_renderer_create_fallback_depth_cube_array(WGPUTextureView *, WGPUDevice *,
                                                WGPUQueue *);
static inline void
scene_renderer_create_fallback_depth_2d_array(WGPUTextureView *, WGPUDevice *,
                                              WGPUQueue *);

/**
   Create and store the basic fallback texture set as placeholder in more
   complex shader like PBR before being replace by computational intensive
   process like shadow map or AO baking.
 */
void scene_renderer_init_fallback_textures(SceneRenderer *renderer) {

  VERBOSE_PROCESS("Caching fallback textures...");

  WGPUDevice *device = &renderer->wgpu.device;
  WGPUQueue *queue = &renderer->wgpu.queue;

  // create texture 2D fallback
  scene_renderer_create_fallback_texture_2d(
      &renderer->texture.fallback.texture_2d, device, queue);

  // create texture 2D view fallback
  scene_renderer_create_fallback_texture_2d_view(
      &renderer->texture.fallback.texture_2d_view, device, queue);

  // create depth cube array view fallback
  scene_renderer_create_fallback_depth_cube_array(
      &renderer->texture.fallback.depth_cube_array_view, device, queue);

  // create depth 2d array view fallback
  scene_renderer_create_fallback_depth_2d_array(
      &renderer->texture.fallback.depth_2d_array_view, device, queue);
}

void scene_renderer_create_fallback_texture_2d(Texture *texture,
                                               WGPUDevice *device,
                                               WGPUQueue *queue) {
  texture_create(texture, &(TextureCreateDescriptor){
                              .channels = 4,
                              .width = 1,
                              .height = 1,
                              .value = (uint8_t[]){0, 0, 0, 255},
                          });
}

void scene_renderer_create_fallback_texture_2d_view(WGPUTextureView *view,
                                                    WGPUDevice *device,
                                                    WGPUQueue *queue) {

  buffer_create_texture(view,
                        &(CreateTextureDescriptor){
                            .device = device,
                            .queue = queue,
                            .width = 1,
                            .height = 1,
                            .channels = 1,
                            .format = WGPUTextureFormat_R8Unorm,
                            .data = (uint8_t[]){255},
                            .size = sizeof(uint32_t),
                        },
                        BufferTextureMemory_Keep);
}

void scene_renderer_create_fallback_depth_cube_array(WGPUTextureView *view,
                                                     WGPUDevice *device,
                                                     WGPUQueue *queue) {

  // create texture
  WGPUTexture texture = wgpuDeviceCreateTexture(
      *device, &(WGPUTextureDescriptor){
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

  // assign to texture view
  *view = wgpuTextureCreateView(
      texture, &(WGPUTextureViewDescriptor){
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

void scene_renderer_create_fallback_depth_2d_array(WGPUTextureView *view,
                                                   WGPUDevice *device,
                                                   WGPUQueue *queue) {

  // create texture
  WGPUTexture texture = wgpuDeviceCreateTexture(
      *device,
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

  // assign to texture view
  *view = wgpuTextureCreateView(
      texture, &(WGPUTextureViewDescriptor){
                   .label = "Fallback 2D array view",
                   .dimension = WGPUTextureViewDimension_2DArray,
                   .format = SHADOW_DEPTH_FORMAT,
                   .baseMipLevel = 0,
                   .mipLevelCount = 1,
                   .baseArrayLayer = 0,
                   .arrayLayerCount = 1, // or however many layers you define
                   .aspect = WGPUTextureAspect_DepthOnly,
               });
}
