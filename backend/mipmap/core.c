#include "core.h"
#include "backend/std_pipeline/core.h"
#include "runtime/shader/core.h"
#include "runtime/texture/core.h"
#include "utils/system.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <math.h>
#include <stdint.h>

static inline void mipmap_draw(WGPUTexture, const MipmapCreateDescriptor *);

static inline void mipmap_dispatch(WGPUComputePassEncoder,
                                   const TextureResolution,
                                   const TextureResolution, const mip_t);

MipmapStatus mipmap_create(WGPUTexture texture,
                           const MipmapCreateDescriptor *desc) {

  VERBOSE_PROCESS("Generating mipmaps...");

  TIMER("Mipmap Generation", { mipmap_draw(texture, desc); });

  return MipmapStatus_Success;
}

void mipmap_draw(WGPUTexture texture, const MipmapCreateDescriptor *desc) {

  WGPUCommandEncoder command_encoder =
      wgpuDeviceCreateCommandEncoder(desc->device, NULL);

  const mip_t mip_count = wgpuTextureGetMipLevelCount(texture);

  const WGPUComputePipeline mipmap_pipeline =
      std_compute_pipeline(ComputePipelineType_Mipmap)->handle;

  const uint32_t layer_count =
      desc->dimension == WGPUTextureViewDimension_Cube ? 6 : 1;

  WGPUSampler mipmap_sampler = wgpuDeviceCreateSampler(
      desc->device, &(WGPUSamplerDescriptor){
                        .label = "Mipmap sampler",
                        .addressModeU = WGPUAddressMode_ClampToEdge,
                        .addressModeV = WGPUAddressMode_ClampToEdge,
                        .addressModeW = WGPUAddressMode_ClampToEdge,
                        .magFilter = WGPUFilterMode_Linear,
                        .minFilter = WGPUFilterMode_Linear,
                        .mipmapFilter = WGPUMipmapFilterMode_Linear,
                        .lodMinClamp = 0.0f,
                        .lodMaxClamp = (float)mip_count,
                    });

  for (uint32_t i = 0; i < layer_count; i++) {
    for (mip_t j = 1; j < mip_count; j++) {

      /* === PASS  BEGIN === */
      WGPUComputePassEncoder compute_pass = wgpuCommandEncoderBeginComputePass(
          command_encoder,
          &(WGPUComputePassDescriptor){.label = "Mipmap Compute Pass"});

      wgpuComputePassEncoderSetPipeline(compute_pass, mipmap_pipeline);

      /* === BIND TARGET VIEWS === */
      WGPUTextureViewDescriptor src_view_desc = {
          .format = desc->format,
          .dimension = WGPUTextureViewDimension_2D,
          .baseMipLevel = j - 1,
          .mipLevelCount = 1,
          .baseArrayLayer = i,
          .arrayLayerCount = 1,
      };

      WGPUTextureViewDescriptor dst_view_desc = src_view_desc;
      dst_view_desc.baseMipLevel = j;

      WGPUTextureView src_view = wgpuTextureCreateView(texture, &src_view_desc);
      WGPUTextureView dst_view = wgpuTextureCreateView(texture, &dst_view_desc);

      WGPUBindGroupEntry entries[3] = {
          {.binding = 0, .textureView = src_view},
          {.binding = 1, .sampler = mipmap_sampler},
          {.binding = 2, .textureView = dst_view},
      };

      WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(
          desc->device, &(WGPUBindGroupDescriptor){
                            .layout = wgpuComputePipelineGetBindGroupLayout(
                                mipmap_pipeline, 0),
                            .entryCount = 3,
                            .entries = entries,
                        });

      wgpuComputePassEncoderSetBindGroup(compute_pass, 0, bind_group, 0, NULL);

      mipmap_dispatch(compute_pass, desc->width, desc->height, j);

      /* === PASS END === */
      wgpuComputePassEncoderEnd(compute_pass);
      wgpuComputePassEncoderRelease(compute_pass);

      wgpuBindGroupRelease(bind_group);

      wgpuTextureViewRelease(src_view);
      wgpuTextureViewRelease(dst_view);
    }
  }
  WGPUCommandBuffer compute_buffer =
      wgpuCommandEncoderFinish(command_encoder, NULL);

  wgpuQueueSubmit(desc->queue, 1, &compute_buffer);

  wgpuCommandEncoderRelease(command_encoder);
  wgpuCommandBufferRelease(compute_buffer);
  wgpuSamplerRelease(mipmap_sampler);
}

void mipmap_dispatch(WGPUComputePassEncoder pass, const TextureResolution width,
                     const TextureResolution height, const mip_t mip) {

  mip_t mip_width = glm_max(1, width >> mip);
  mip_t mip_height = glm_max(1, height >> mip);

  uint32_t dispatch_x = (mip_width + MIPMAP_WORKGROUP - 1) / MIPMAP_WORKGROUP;
  uint32_t dispatch_y = (mip_height + MIPMAP_WORKGROUP - 1) / MIPMAP_WORKGROUP;

  wgpuComputePassEncoderDispatchWorkgroups(pass, dispatch_x, dispatch_y, 1);
}
