#include "mipmap.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/resource_manager.h"
#include "backend/std_pipeline/core.h"
#include "runtime/shader/core.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <math.h>
#include <stdint.h>

static inline void compute_pass_mipmap_draw(ComputePass *,
                                            const MipmapDescriptor *);

static inline void compute_pass_mipmap_dispatch(WGPUComputePassEncoder,
                                                const TextureResolution,
                                                const TextureResolution,
                                                const mip_t);

MipmapStatus compute_pass_mipmap(ComputePass *pass,
                                 const MipmapDescriptor *desc) {

  // logger_add(LoggerFlag_Process, "Generating mipmaps...");
  // TIMER("Mipmap Generation", {});
  compute_pass_mipmap_draw(pass, desc);

  return MipmapStatus_Success;
}

void compute_pass_mipmap_draw(ComputePass *pass, const MipmapDescriptor *desc) {

  WGPUCommandEncoder command_encoder =
      wgpuDeviceCreateCommandEncoder(context_device(), NULL);

  const mip_t mip_count = wgpuTextureGetMipLevelCount(desc->texture);

  const WGPUComputePipeline mipmap_pipeline =
      std_compute_pipeline(ComputePipelineType_Mipmap)->handle;

  for (uint32_t i = 0; i < desc->layer_count; i++) {
    for (mip_t j = 1; j < mip_count; j++) {

      /* === PASS  BEGIN === */
      WGPUComputePassEncoder compute_pass = wgpuCommandEncoderBeginComputePass(
          command_encoder,
          &(WGPUComputePassDescriptor){.label = "Mipmap Compute Pass"});

      wgpuComputePassEncoderSetPipeline(compute_pass, mipmap_pipeline);

      /* === BIND TARGET VIEWS === */
      WGPUTextureViewDescriptor src_view_desc = {
          .format = wgpuTextureGetFormat(desc->texture),
          .dimension = WGPUTextureViewDimension_2D,
          .baseMipLevel = j - 1,
          .mipLevelCount = 1,
          .baseArrayLayer = i,
          .arrayLayerCount = 1,
      };

      WGPUTextureViewDescriptor dst_view_desc = src_view_desc;
      dst_view_desc.baseMipLevel = j;

      WGPUTextureView src_view = rem_new_view(desc->texture, &src_view_desc);
      WGPUTextureView dst_view = rem_new_view(desc->texture, &dst_view_desc);

      WGPUBindGroupEntry entries[3] = {
          {.binding = 0, .textureView = src_view},
          {.binding = 1, .sampler = pass->sampler},
          {.binding = 2, .textureView = dst_view},
      };

      WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(
          context_device(), &(WGPUBindGroupDescriptor){
                                .layout = wgpuComputePipelineGetBindGroupLayout(
                                    mipmap_pipeline, 0),
                                .entryCount = 3,
                                .entries = entries,
                            });

      wgpuComputePassEncoderSetBindGroup(compute_pass, 0, bind_group, 0, NULL);

      compute_pass_mipmap_dispatch(compute_pass,
                                   wgpuTextureGetWidth(desc->texture),
                                   wgpuTextureGetHeight(desc->texture), j);

      /* === PASS END === */
      wgpuComputePassEncoderEnd(compute_pass);
      wgpuComputePassEncoderRelease(compute_pass);

      wgpuBindGroupRelease(bind_group);

      rem_destroy_view(&src_view);
      rem_destroy_view(&dst_view);
    }
  }
  WGPUCommandBuffer compute_buffer =
      wgpuCommandEncoderFinish(command_encoder, NULL);

  wgpuQueueSubmit(context_queue(), 1, &compute_buffer);

  wgpuCommandEncoderRelease(command_encoder);
  wgpuCommandBufferRelease(compute_buffer);
}

void compute_pass_mipmap_dispatch(WGPUComputePassEncoder pass,
                                  const TextureResolution width,
                                  const TextureResolution height,
                                  const mip_t mip) {

  mip_t mip_width = glm_max(1, width >> mip);
  mip_t mip_height = glm_max(1, height >> mip);

  uint32_t dispatch_x = (mip_width + MIPMAP_WORKGROUP - 1) / MIPMAP_WORKGROUP;
  uint32_t dispatch_y = (mip_height + MIPMAP_WORKGROUP - 1) / MIPMAP_WORKGROUP;

  wgpuComputePassEncoderDispatchWorkgroups(pass, dispatch_x, dispatch_y, 1);
}
