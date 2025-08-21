#include "core.h"

#include "../shadow_map/shadow_map.h"
#include "../utils/system.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

WGPUTextureView g_std_texture_view[STD_TEXTURE_VIEW_COUNT] = {0};
WGPUTexture g_std_texture[STD_TEXTURE_VIEW_COUNT] = {0};

// black color
static const uint8_t pixel[4] = {255, 255, 255, 255};

static inline WGPUTextureView
scene_renderer_create_fallback_float(WGPUTexture *, const WGPUDevice,
                                     const WGPUQueue);

static inline WGPUTextureView
scene_renderer_create_fallback_float_cube(WGPUTexture *, const WGPUDevice,
                                          const WGPUQueue);

static inline WGPUTextureView scene_renderer_create_fallback_float_cube_array(
    WGPUTexture *, const WGPUDevice, const WGPUQueue);

static inline WGPUTextureView
scene_renderer_create_fallback_depth(WGPUTexture *, const WGPUDevice,
                                     const WGPUQueue);

static inline WGPUTextureView
scene_renderer_create_fallback_depth_cube_array(WGPUTexture *, const WGPUDevice,
                                                const WGPUQueue);

static inline WGPUTextureView
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

static const std_texture_view_create texture_creator[STD_TEXTURE_VIEW_COUNT] = {
    [TextureViewType_Float] = scene_renderer_create_fallback_float,
    [TextureViewType_Depth] = scene_renderer_create_fallback_depth,
    [TextureViewType_FloatCube] = scene_renderer_create_fallback_float_cube,
    [TextureViewType_FloatCubeArray] =
        scene_renderer_create_fallback_float_cube_array,
    [TextureViewType_DepthCubeArray] =
        scene_renderer_create_fallback_depth_cube_array,
    [TextureViewType_Depth2DArray] =
        scene_renderer_create_fallback_depth_2d_array,
};

void scene_renderer_init_fallback_textures(const WGPUDevice device,
                                           const WGPUQueue queue) {

  VERBOSE_PROCESS("Caching fallback textures...");

  for (TextureViewType i = 0; i < STD_TEXTURE_VIEW_COUNT; i++)
    g_std_texture_view[i] =
        texture_creator[i](&g_std_texture[i], device, queue);
}

WGPUTextureView scene_renderer_create_fallback_float(WGPUTexture *texture,
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

  return wgpuTextureCreateView(*texture, NULL);
}

WGPUTextureView scene_renderer_create_fallback_float_cube(
    WGPUTexture *texture, const WGPUDevice device, const WGPUQueue queue) {

  *texture = wgpuDeviceCreateTexture(
      device,
      &(WGPUTextureDescriptor){
          .size =
              {
                  .width = 1,
                  .height = 1,
                  .depthOrArrayLayers = 6,
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

  return wgpuTextureCreateView(*texture,
                               &(WGPUTextureViewDescriptor){
                                   .dimension = WGPUTextureViewDimension_Cube,
                                   .format = WGPUTextureFormat_R8Unorm,
                                   .mipLevelCount = 1,
                                   .arrayLayerCount = 6,
                               });
}

WGPUTextureView scene_renderer_create_fallback_float_cube_array(
    WGPUTexture *texture, const WGPUDevice device, const WGPUQueue queue) {

  *texture = wgpuDeviceCreateTexture(
      device,
      &(WGPUTextureDescriptor){
          .size =
              {
                  .width = 1,
                  .height = 1,
                  .depthOrArrayLayers = 6,
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

  return wgpuTextureCreateView(
      *texture, &(WGPUTextureViewDescriptor){
                    .dimension = WGPUTextureViewDimension_CubeArray,
                    .format = WGPUTextureFormat_R8Unorm,
                    .mipLevelCount = 1,
                    .arrayLayerCount = 6,
                });
}

WGPUTextureView scene_renderer_create_fallback_depth(WGPUTexture *texture,
                                                     const WGPUDevice device,
                                                     const WGPUQueue queue) {

  *texture = wgpuDeviceCreateTexture(
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

  return wgpuTextureCreateView(*texture, NULL);
}

WGPUTextureView scene_renderer_create_fallback_depth_cube_array(
    WGPUTexture *texture, const WGPUDevice device, const WGPUQueue queue) {

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

  return wgpuTextureCreateView(
      *texture, &(WGPUTextureViewDescriptor){
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

WGPUTextureView scene_renderer_create_fallback_depth_2d_array(
    WGPUTexture *texture, const WGPUDevice device, const WGPUQueue queue) {

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

  return wgpuTextureCreateView(
      *texture, &(WGPUTextureViewDescriptor){
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

/**
   Traverse the std texture view list and compare the argument and returns true
   if the view corressponds to a standrard texture view.

   This function is mostly used during the shader texture updates as to prevent
   to release a global std texture (making it unavailable for the resources
   using it).
 */
bool is_std_texture_view(const WGPUTextureView view) {

  for (size_t i = 0; i < STD_TEXTURE_VIEW_COUNT; i++)
    if (view == std_texture_view(i))
      return true;

  return false;
}
