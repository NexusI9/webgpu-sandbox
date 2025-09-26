#include "core.h"

#include <stddef.h>
#include <stdint.h>

#include "backend/context.h"
#include "backend/logger.h"
#include "runtime/light/shadow_map/core.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"

WGPUTextureView g_std_texture_view[STD_TEXTURE_VIEW_COUNT] = {0};
WGPUTexture g_std_texture[STD_TEXTURE_VIEW_COUNT] = {0};

// black color
static const uint8_t pixel[4] = {255, 255, 255, 255};

static inline WGPUTextureView
scene_renderer_create_fallback_float(WGPUTexture *);

static inline WGPUTextureView
scene_renderer_create_fallback_float_2d_array(WGPUTexture *);

static inline WGPUTextureView
scene_renderer_create_fallback_float_cube(WGPUTexture *);

static inline WGPUTextureView
scene_renderer_create_fallback_float_cube_array(WGPUTexture *);

static inline WGPUTextureView
scene_renderer_create_fallback_depth(WGPUTexture *);

static inline WGPUTextureView
scene_renderer_create_fallback_depth_cube_array(WGPUTexture *);

static inline WGPUTextureView
scene_renderer_create_fallback_depth_2d_array(WGPUTexture *);

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
    // float
    [TextureViewType_Float] = scene_renderer_create_fallback_float,
    [TextureViewType_Float2DArray] =
        scene_renderer_create_fallback_float_2d_array,
    [TextureViewType_FloatCube] = scene_renderer_create_fallback_float_cube,
    [TextureViewType_FloatCubeArray] =
        scene_renderer_create_fallback_float_cube_array,
    // depth
    [TextureViewType_Depth] = scene_renderer_create_fallback_depth,
    [TextureViewType_DepthCubeArray] =
        scene_renderer_create_fallback_depth_cube_array,
    [TextureViewType_Depth2DArray] =
        scene_renderer_create_fallback_depth_2d_array,
};

void standard_textures_init() {

  logger_add(LoggerFlag_Process, "Caching fallback textures...");

  for (TextureViewType i = 0; i < STD_TEXTURE_VIEW_COUNT; i++)
    g_std_texture_view[i] = texture_creator[i](&g_std_texture[i]);
}

WGPUTextureView scene_renderer_create_fallback_float(WGPUTexture *texture) {

  *texture = wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .label = "Standard Texture Float 2D",
          .size =
              {
                  .width = 1,
                  .height = 1,
                  .depthOrArrayLayers = 1,
              },
          .format = TEXTURE_FORMAT_OFFSCREEN,
          .mipLevelCount = 1,
          .sampleCount = 1,
          .dimension = WGPUTextureDimension_2D,
          .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
      });

  wgpuQueueWriteTexture(context_queue(),
                        &(WGPUImageCopyTexture){
                            .texture = *texture,
                            .mipLevel = 0,
                            .origin = {0, 0, 0},
                            .aspect = WGPUTextureAspect_All,
                        },
                        pixel, 4 * sizeof(uint32_t),
                        &(WGPUTextureDataLayout){
                            .offset = 0,
                            .bytesPerRow = 4,
                            .rowsPerImage = 1,
                        },
                        &(WGPUExtent3D){1, 1, 1});

  return wgpuTextureCreateView(*texture, NULL);
}

WGPUTextureView
scene_renderer_create_fallback_float_2d_array(WGPUTexture *texture) {

  *texture = wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .label = "Standard Texture Float Array",
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

  // DELETEME?
  wgpuQueueWriteTexture(context_queue(),
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
                    .label = "Standard View Float Array",
                    .dimension = WGPUTextureViewDimension_2DArray,
                    .format = WGPUTextureFormat_R8Unorm,
                    .mipLevelCount = 1,
                    .arrayLayerCount = 1,
                });
}

WGPUTextureView
scene_renderer_create_fallback_float_cube(WGPUTexture *texture) {

  *texture = wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .label = "Standard Texture Float Cube",
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

  // DELETEME?
  wgpuQueueWriteTexture(context_queue(),
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
                                   .label = "Standard View Float Cube",
                                   .dimension = WGPUTextureViewDimension_Cube,
                                   .format = WGPUTextureFormat_R8Unorm,
                                   .mipLevelCount = 1,
                                   .arrayLayerCount = 6,
                               });
}

WGPUTextureView
scene_renderer_create_fallback_float_cube_array(WGPUTexture *texture) {

  *texture = wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .label = "Standard Texture Float Cube Array",
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

  wgpuQueueWriteTexture(context_queue(),
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
                    .label = "Standard View Float Cube Array",
                    .dimension = WGPUTextureViewDimension_CubeArray,
                    .format = WGPUTextureFormat_R8Unorm,
                    .mipLevelCount = 1,
                    .arrayLayerCount = 6,
                });
}

WGPUTextureView scene_renderer_create_fallback_depth(WGPUTexture *texture) {

  *texture = wgpuDeviceCreateTexture(
      context_device(), &(WGPUTextureDescriptor){
                            .label = "Standard Texture Depth 2D",
                            .size =
                                {
                                    .width = 1,
                                    .height = 1,
                                    .depthOrArrayLayers = 1,
                                },
                            .format = TEXTURE_FORMAT_DEPTH,
                            .mipLevelCount = 1,
                            .sampleCount = 1,
                            .dimension = WGPUTextureDimension_2D,
                            .usage = WGPUTextureUsage_TextureBinding |
                                     WGPUTextureUsage_RenderAttachment,
                        });

  return wgpuTextureCreateView(*texture, NULL);
}

WGPUTextureView
scene_renderer_create_fallback_depth_cube_array(WGPUTexture *texture) {

  // create texture
  *texture = wgpuDeviceCreateTexture(
      context_device(), &(WGPUTextureDescriptor){
                            .label = "Standard Texture Depth Cube Array",
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
                    .label = "Standard View Depth Cube Array",
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
scene_renderer_create_fallback_depth_2d_array(WGPUTexture *texture) {

  // create texture
  *texture = wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .label = "Standard Texture Cube Depth 2D Array",
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
                    .label = "Standard View Cube Depth 2D Array",
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
