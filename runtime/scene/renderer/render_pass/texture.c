#include "texture.h"

#include <stddef.h>

#include "backend/context.h"
#include "backend/logger.h"
#include "runtime/pipeline/render.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"

/**

   ▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▖ ▗▖▗▄▄▖ ▗▄▄▄▖ ▗▄▄▖
     █  ▐▌    ▝▚▞▘   █  ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌
     █  ▐▛▀▀▘  ▐▌    █  ▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘ ▝▀▚▖
     █  ▐▙▄▄▖▗▞▘▝▚▖  █  ▝▚▄▞▘▐▌ ▐▌▐▙▄▄▖▗▄▄▞▘

   Create the texture and texture view for the multisampling rendering.

 */
void render_pass_create_multisampling_view(
    WGPUTexture *texture, WGPUTextureView *view,
    const RenderPassTextureDescriptor *desc) {

  if (desc->multisample == 0) {
    logger_add(LoggerFlag_Warning,
               "Multisample provided is not valid (%d), make sure the "
               "render pass is correctly initialised.",
               desc->multisample);
    return;
  }

  *texture = wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .label = "MSAA Texture",
          .usage = WGPUTextureUsage_TextureBinding |
                   WGPUTextureUsage_RenderAttachment,
          .size =
              (WGPUExtent3D){
                  .width = desc->width,
                  .height = desc->height,
                  .depthOrArrayLayers = 1,
              },
          .format = TEXTURE_FORMAT_ONSCREEN_DEFAULT, // swapchain format
          .sampleCount = desc->multisample,
          .mipLevelCount = 1,
      });

  *view = wgpuTextureCreateView(*texture, NULL);
}

/*
   Pass 1---.
   Pass 2---+--=> MSAA (4x) => RESOLVE (1X) => BLIT => SWAPCHAIN
   Pass 3---'
 */
void render_pass_create_resolve_view(WGPUTexture *texture,
                                     WGPUTextureView *view,
                                     const RenderPassTextureDescriptor *desc) {

  *texture = wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .label = "Resolve Texture",
          .usage = WGPUTextureUsage_TextureBinding |
                   WGPUTextureUsage_RenderAttachment,
          .size =
              (WGPUExtent3D){
                  .width = desc->width,
                  .height = desc->height,
                  .depthOrArrayLayers = 1,
              },
          .format = TEXTURE_FORMAT_ONSCREEN_DEFAULT, // swapchain format
          .sampleCount = PipelineMultisampleCount_1x,
          .mipLevelCount = 1,
      });

  *view = wgpuTextureCreateView(*texture, NULL);
}

void render_pass_create_depth_view(WGPUTexture *texture, WGPUTextureView *view,
                                   const RenderPassTextureDescriptor *desc) {

  // Need to create a texture view for Z buffer stencil
  // by default set depth based on draw call order (first ones in
  // backgrounds...)
  // => Need to create a depth texture: a hidden buffer storing depth values for
  // each pixel
  *texture = wgpuDeviceCreateTexture(
      context_device(),
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
      *texture, &(WGPUTextureViewDescriptor){
                    .format = WGPUTextureFormat_Depth24Plus,
                    .dimension = WGPUTextureViewDimension_2D,
                    .baseMipLevel = 0,
                    .mipLevelCount = 1, // match above texture
                    .baseArrayLayer = 0,
                    .arrayLayerCount = 1, // not using array texture (only 1)
                    .aspect = WGPUTextureAspect_DepthOnly,
                });
}
