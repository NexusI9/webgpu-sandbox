#include "core.h"

#include "../shadow_map/shadow_map.h"
#include "../utils/system.h"
#include "webgpu/webgpu.h"

WGPUTextureView g_std_texture_view[STD_TEXTURE_VIEW_COUNT] = {0};

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

static inline WGPUTextureView
scene_renderer_create_fallback_float(const WGPUDevice, const WGPUQueue);

static inline WGPUTextureView
scene_renderer_create_fallback_depth(const WGPUDevice, const WGPUQueue);

static inline WGPUTextureView
scene_renderer_create_fallback_depth_cube_array(const WGPUDevice,
                                                const WGPUQueue);

static inline WGPUTextureView
scene_renderer_create_fallback_depth_2d_array(const WGPUDevice,
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
void scene_renderer_init_fallback_textures(const WGPUDevice device,
                                           const WGPUQueue queue) {

  VERBOSE_PROCESS("Caching fallback textures...");

  // create texture 2D float view fallback
  g_std_texture_view[TextureViewType_Float] =
      scene_renderer_create_fallback_float(device, queue);

  // create texture 2D depth view fallback
  g_std_texture_view[TextureViewType_Depth] =
      scene_renderer_create_fallback_depth(device, queue);

  // create depth cube array view fallback
  g_std_texture_view[TextureViewType_DepthCubeArray] =
      scene_renderer_create_fallback_depth_cube_array(device, queue);

  // create depth 2d array view fallback
  g_std_texture_view[TextureViewType_Depth2DArray] =
      scene_renderer_create_fallback_depth_2d_array(device, queue);
}

WGPUTextureView scene_renderer_create_fallback_float(const WGPUDevice device,
                                                     const WGPUQueue queue) {

  WGPUTexture texture = wgpuDeviceCreateTexture(
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
                            .texture = texture,
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

  return wgpuTextureCreateView(texture, NULL);
}

WGPUTextureView scene_renderer_create_fallback_depth(const WGPUDevice device,
                                                     const WGPUQueue queue) {

  WGPUTexture texture = wgpuDeviceCreateTexture(
      device, &(WGPUTextureDescriptor){
                  .size =
                      {
                          .width = 1,
                          .height = 1,
                          .depthOrArrayLayers = 1,
                      },
                  .format = WGPUTextureFormat_Depth24Plus,
                  .mipLevelCount = 1,
                  .sampleCount = 1,
                  .dimension = WGPUTextureDimension_2D,
                  .usage = WGPUTextureUsage_TextureBinding |
                           WGPUTextureUsage_RenderAttachment,
              });

  return wgpuTextureCreateView(texture, NULL);
}

WGPUTextureView
scene_renderer_create_fallback_depth_cube_array(const WGPUDevice device,
                                                const WGPUQueue queue) {

  // create texture
  WGPUTexture texture = wgpuDeviceCreateTexture(
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

  return wgpuTextureCreateView(
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

WGPUTextureView
scene_renderer_create_fallback_depth_2d_array(const WGPUDevice device,
                                              const WGPUQueue queue) {

  // create texture
  WGPUTexture texture = wgpuDeviceCreateTexture(
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

  return wgpuTextureCreateView(
      texture, &(WGPUTextureViewDescriptor){
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

const WGPUTextureView std_texture_view(const TextureViewType type) {
  return g_std_texture_view[type];
}
