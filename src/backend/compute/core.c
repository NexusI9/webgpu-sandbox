#include "core.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/resource_manager.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <string.h>

/**
   Initialize the key/commons elements of the differents compute passes (kawase,
   mipmaps) at ot prevent create/destroy those during the pass call which often
   happens in hot path so we can't afford extensive gpu write.

   For the texture destination we basically create a texture that has the size
   of the swapchain as the max size and then proceed to draw on it in certain
   region. It acts as a temporary canvas on which we may draw stuff during the
   compute process.
 */
ComputePassStatus compute_pass_init(ComputePass *pass,
                                    const ComputePassDescriptor *desc) {

  logger_add(LoggerFlag_Process, "Initializing Renderer Compute Pass");

  pass->sampler = rem_new_sampler(&(WGPUSamplerDescriptor){
      .label = "Compute Pass Common Sampler",
      .addressModeU = WGPUAddressMode_ClampToEdge,
      .addressModeV = WGPUAddressMode_ClampToEdge,
      .addressModeW = WGPUAddressMode_ClampToEdge,
      .magFilter = WGPUFilterMode_Linear,
      .minFilter = WGPUFilterMode_Linear,
      .mipmapFilter = WGPUMipmapFilterMode_Linear,
  });

  const int max_dim = glm_max(desc->max_width, desc->max_height);
  pass->destination_texture = rem_new_texture(&(WGPUTextureDescriptor){
      .label = "Compute Pass Destination Texture",
      .dimension = WGPUTextureDimension_2D,
      .size = (WGPUExtent3D){max_dim, max_dim, 1},
      .format = TEXTURE_FORMAT_OFFSCREEN,
      .mipLevelCount = 1,
      .sampleCount = 1,
      .usage = WGPUTextureUsage_TextureBinding |
               WGPUTextureUsage_StorageBinding | WGPUTextureUsage_CopySrc |
               WGPUTextureUsage_CopyDst,

  });

  pass->buffer = rem_new_buffer(&(WGPUBufferDescriptor){
      .label = "Compute Pass Common Buffer",
      .usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst,
      .mappedAtCreation = false,
      .size = COMPUTE_PASS_BUFFER_MAX_SIZE,
  });

  return ComputePassStatus_Success;
}
