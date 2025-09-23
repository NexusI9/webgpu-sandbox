#include "kawase.h"
#include "backend/buffer.h"
#include "backend/std_pipeline/core.h"
#include "runtime/shader/core.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <math.h>
#include <stdint.h>

static inline void compute_pass_kawase_draw(ComputePass *,
                                            const KawaseDescriptor *);

static inline void compute_pass_kawase_dispatch(WGPUComputePassEncoder,
                                                const TextureResolution,
                                                const TextureResolution);

KawaseStatus compute_pass_kawase(ComputePass *pass,
                                 const KawaseDescriptor *desc) {

  compute_pass_kawase_draw(pass, desc);
  return KawaseStatus_Success;
}


void compute_pass_kawase_draw(ComputePass *pass, const KawaseDescriptor *desc) {

  WGPUCommandEncoder command_encoder =
      wgpuDeviceCreateCommandEncoder(desc->device, NULL);

  const ComputePipeline *compute_pipeline =
      std_compute_pipeline(ComputePipelineType_Kawase);

  const WGPUComputePipeline pipeline = compute_pipeline->handle;
  const WGPUBindGroupLayout bind_group_layout =
      wgpuComputePipelineGetBindGroupLayout(pipeline, 0);

  const TextureResolution width = wgpuTextureGetWidth(desc->texture);
  const TextureResolution height = wgpuTextureGetHeight(desc->texture);

  KawaseUniform uniform = {
      .texel_size =
          {
              g_texture_resolution_texel_size[width],
              g_texture_resolution_texel_size[height],
          },
      .offset = 1,
  };

  WGPUTexture a_tex = desc->texture;
  WGPUTexture b_tex = pass->destination_texture;

  for (uint32_t i = 0; i < desc->layer_count; i++) {

    WGPUTextureViewDescriptor a_desc = {
        .format = wgpuTextureGetFormat(desc->texture),
        .dimension = WGPUTextureViewDimension_2D,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = i,
        .arrayLayerCount = 1,
    };

    WGPUTextureViewDescriptor b_desc = a_desc;
    b_desc.baseArrayLayer = 0; // reset array

    WGPUTextureView a_view = wgpuTextureCreateView(a_tex, &a_desc);
    WGPUTextureView b_view = wgpuTextureCreateView(b_tex, &b_desc);

    WGPUBindGroupEntry entries[4] = {
        {.binding = 0, .textureView = a_view},
        {.binding = 1, .sampler = pass->sampler},
        {.binding = 2, .textureView = b_view},
        {.binding = 3, .buffer = pass->buffer, .size = sizeof(KawaseUniform)},
    };

    // cache 2 bind groups per layer
    WGPUBindGroup bind_group_a =
        wgpuDeviceCreateBindGroup(desc->device, &(WGPUBindGroupDescriptor){
                                                    .layout = bind_group_layout,
                                                    .entryCount = 4,
                                                    .entries = entries,
                                                });

    entries[0].textureView = b_view;
    entries[2].textureView = a_view;
    WGPUBindGroup bind_group_b =
        wgpuDeviceCreateBindGroup(desc->device, &(WGPUBindGroupDescriptor){
                                                    .layout = bind_group_layout,
                                                    .entryCount = 4,
                                                    .entries = entries,
                                                });

    for (uint32_t j = 0; j < desc->pass_count; j++) {

      /* === PASS  BEGIN === */
      WGPUComputePassEncoder compute_pass = wgpuCommandEncoderBeginComputePass(
          command_encoder,
          &(WGPUComputePassDescriptor){.label = "Kawase Compute Pass"});

      wgpuComputePassEncoderSetPipeline(compute_pass, pipeline);

      // update offset uniform
      wgpuQueueWriteBuffer(desc->queue, pass->buffer, 0, &uniform,
                           sizeof(KawaseUniform));

      wgpuComputePassEncoderSetBindGroup(
          compute_pass, 0, (j % 2 == 0) ? bind_group_a : bind_group_b, 0, NULL);

      compute_pass_kawase_dispatch(compute_pass,
                                   wgpuTextureGetWidth(desc->texture),
                                   wgpuTextureGetHeight(desc->texture));

      /* === PASS END === */
      wgpuComputePassEncoderEnd(compute_pass);
      wgpuComputePassEncoderRelease(compute_pass);

      uniform.offset++;
    }

    // copy dest to src if count pass is odd (since result end up in dest)
    if (desc->pass_count % 2 == 1) {
      wgpuCommandEncoderCopyTextureToTexture(
          command_encoder,
          &(WGPUImageCopyTexture){
              .texture = b_tex,
              .mipLevel = 0,
              .aspect = WGPUTextureAspect_All,
          },
          &(WGPUImageCopyTexture){.texture = a_tex,
                                  .mipLevel = 0,
                                  .origin = {0, 0, i},
                                  .aspect = WGPUTextureAspect_All},
          &(WGPUExtent3D){width, height, 1});
    }

    wgpuBindGroupRelease(bind_group_a);
    wgpuBindGroupRelease(bind_group_b);

    wgpuTextureViewRelease(a_view);
    wgpuTextureViewRelease(b_view);
  }

  WGPUCommandBuffer compute_buffer =
      wgpuCommandEncoderFinish(command_encoder, NULL);

  wgpuQueueSubmit(desc->queue, 1, &compute_buffer);

  wgpuCommandEncoderRelease(command_encoder);
  wgpuCommandBufferRelease(compute_buffer);
}

void compute_pass_kawase_dispatch(WGPUComputePassEncoder pass,
                                  const TextureResolution width,
                                  const TextureResolution height) {

  uint32_t dispatch_x = (width + KAWASE_WORKGROUP - 1) / KAWASE_WORKGROUP;
  uint32_t dispatch_y = (height + KAWASE_WORKGROUP - 1) / KAWASE_WORKGROUP;

  wgpuComputePassEncoderDispatchWorkgroups(pass, dispatch_x, dispatch_y, 1);
}
