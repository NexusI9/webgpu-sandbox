#include "texture.h"
#include "./core.h"

/**
   Create the two shadow textures (color and depth) for the point lights
   In our semantic we use both terms Maps and Textures, however they both
   serve different purpose.
   - The Shadow Texture holds the Shadow mapping.
   - The Shadow map is the result from our rendering.
 */
void shadow_pass_texture_create(const ShadowPassTextureDescriptor *desc) {

  // texture
  WGPUTextureDescriptor texture_descriptor_base = {
      .size =
          (WGPUExtent3D){
              .width = desc->width,
              .height = desc->height,
              .depthOrArrayLayers = desc->layer_count,
          },
      .usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_RenderAttachment |
               WGPUTextureUsage_TextureBinding,
      .dimension = WGPUTextureDimension_2D,
      .mipLevelCount = 1,
      .sampleCount = 1,
  };

  WGPUTextureDescriptor texture_descriptor_color = texture_descriptor_base;
  texture_descriptor_color.label = "Light shadow texture - Color";
  texture_descriptor_color.format = SHADOW_COLOR_FORMAT;

  WGPUTextureDescriptor texture_descriptor_depth = texture_descriptor_base;
  texture_descriptor_depth.label = "Light shadow texture - Depth";
  texture_descriptor_depth.format = SHADOW_DEPTH_FORMAT;

  // setup global texture view
  WGPUTextureViewDescriptor texture_view_descriptor_base = {
      .label = "Light Shadow: global texture view - Depth",
      .format = SHADOW_DEPTH_FORMAT,
      .dimension = desc->dimension, // cube for point | array 2d for dir
      .mipLevelCount = 1,
      .baseMipLevel = 0,
      .arrayLayerCount = desc->layer_count,
      .baseArrayLayer = 0,
      .aspect = WGPUTextureAspect_DepthOnly,
  };

  // Create color texture
  *desc->color.texture =
      wgpuDeviceCreateTexture(desc->device, &texture_descriptor_color);
  *desc->color.texture_view = wgpuTextureCreateView(
      *desc->color.texture,
      &(WGPUTextureViewDescriptor){
          .label = "Light Shadow: global texture view - Color",
          .format = SHADOW_COLOR_FORMAT,
          .dimension = desc->dimension,
          .mipLevelCount = 1,
          .baseMipLevel = 0,
          .arrayLayerCount = desc->layer_count,
          .baseArrayLayer = 0,
          .aspect = WGPUTextureAspect_Undefined,
      });

  // Setup light depth texture

  // Create depth texture
  *desc->depth.texture =
      wgpuDeviceCreateTexture(desc->device, &texture_descriptor_depth);
  *desc->depth.texture_view = wgpuTextureCreateView(
      *desc->depth.texture,
      &(WGPUTextureViewDescriptor){
          .label = "Light Shadow: global texture view - Depth",
          .format = SHADOW_DEPTH_FORMAT,
          .dimension = desc->dimension,
          .mipLevelCount = 1,
          .baseMipLevel = 0,
          .arrayLayerCount = desc->layer_count,
          .baseArrayLayer = 0,
          .aspect = WGPUTextureAspect_DepthOnly,
      });

}
