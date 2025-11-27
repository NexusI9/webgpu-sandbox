#include "core.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/resource_manager.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include <string.h>

ComputePassStatus compute_pass_create(ComputePass *pass,
                                      const ComputePassDescriptor *desc) {

  pass->label = desc->label;
  pass->source_texture = desc->source_texture;
  pass->width = wgpuTextureGetWidth(pass->source_texture);
  pass->height = wgpuTextureGetHeight(pass->source_texture);
  pass->layer_count = wgpuTextureGetDepthOrArrayLayers(pass->source_texture);
  pass->buffer_texture = desc->buffer_texture;

  pass->sampler = rem_new_sampler(&(WGPUSamplerDescriptor){
      .label = "Compute Pass Sampler",
      .addressModeU = WGPUAddressMode_ClampToEdge,
      .addressModeV = WGPUAddressMode_ClampToEdge,
      .addressModeW = WGPUAddressMode_ClampToEdge,
      .magFilter = WGPUFilterMode_Linear,
      .minFilter = WGPUFilterMode_Linear,
      .mipmapFilter = WGPUMipmapFilterMode_Linear,
  });

  if (pass->buffer_texture == NULL)
    pass->buffer_texture = rem_new_texture(&(WGPUTextureDescriptor){
        .label = "Compute Pass Destination Texture",
        .dimension = WGPUTextureDimension_2D,
        .size = (WGPUExtent3D){pass->width, pass->height, 1},
        .format = TEXTURE_FORMAT_OFFSCREEN,
        .mipLevelCount = 1,
        .sampleCount = 1,
        .usage = WGPUTextureUsage_TextureBinding |
                 WGPUTextureUsage_StorageBinding | WGPUTextureUsage_CopySrc |
                 WGPUTextureUsage_CopyDst,

    });

  memset(pass->buffers, 0, sizeof(pass->buffers));
  memset(pass->bindgroups, 0, sizeof(pass->bindgroups));
  memset(pass->views, 0, sizeof(pass->views));

  return ComputePassStatus_Success;
}

ComputePassStatus compute_pass_destroy(ComputePass *pass) {

  rem_destroy_sampler(&pass->sampler);
  rem_destroy_texture(&pass->buffer_texture);

  for (uint8_t i = 0; i < COMPUTE_PASS_BUFFER_CAPACITY; i++)
    rem_destroy_buffer(&pass->buffers[i]);

  for (uint8_t i = 0; i < COMPUTE_PASS_VIEW_CAPACITY; i++)
    rem_destroy_view(&pass->views[i]);

  for (uint8_t i = 0; i < COMPUTE_PASS_BINDGROUP_CAPACITY; i++)
    if (pass->bindgroups[i]) {
      wgpuBindGroupRelease(pass->bindgroups[i]);
      pass->bindgroups[i] = NULL;
    }

  return ComputePassStatus_Success;
}
