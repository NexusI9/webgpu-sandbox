#ifndef _POST_FX_DRAW_H_
#define _POST_FX_DRAW_H_

#include "backend/compute/kawase.h"
#include "backend/context.h"
#include "core.h"

// Draw
static inline void post_fx_blit_draw(PostFx *fx,
                                     WGPUCommandEncoder command_encoder) {

  WGPURenderPassColorAttachment color_attachment = {
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
      .view = wgpuSwapChainGetCurrentTextureView(context_swapchain()),
      .loadOp = WGPULoadOp_Load,
      .storeOp = WGPUStoreOp_Store,
  };

  WGPURenderPassDescriptor pass_desc = {
      .label = "Blit Resolve Pass",
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
  };

  WGPURenderPassEncoder pass =
      wgpuCommandEncoderBeginRenderPass(command_encoder, &pass_desc);
  {
    // POST FX
    wgpuRenderPassEncoderSetPipeline(
        pass, (*post_fx_effect(fx, PostFxType_Blit)->pipeline)->handle);
    wgpuRenderPassEncoderSetBindGroup(
        pass, 0, post_fx_effect(fx, PostFxType_Blit)->bindgroup, 0, NULL);
    wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
  }

  wgpuRenderPassEncoderEnd(pass);
}

static inline void post_fx_bloom_draw(PostFx *fx,
                                      WGPUCommandEncoder command_encoder) {

  PostFxEffect *effect = post_fx_effect(fx, PostFxType_Bloom);
  WGPURenderPassColorAttachment color_attachment = {
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
      .view = effect->views[PostFxViewIndex_Bloom],
      .loadOp = WGPULoadOp_Load,
      .storeOp = WGPUStoreOp_Store,
  };

  WGPURenderPassDescriptor pass_desc = {
      .label = "Bloom Resolve Pass",
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
  };

  profiler_latency_start(fx->profiler, ProfilerLatencyType_BloomPass);
  {
    WGPURenderPassEncoder pass =
        wgpuCommandEncoderBeginRenderPass(command_encoder, &pass_desc);
    {
      // POST FX
      wgpuRenderPassEncoderSetPipeline(
          pass, (*post_fx_effect(fx, PostFxType_Bloom)->pipeline)->handle);
      wgpuRenderPassEncoderSetBindGroup(pass, 0, effect->bindgroup, 0, NULL);
      wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
    }

    wgpuRenderPassEncoderEnd(pass);

    ComputePass *kawase_pass = &effect->compute_passes[POST_FX_BLOOM_KAWASE];
    compute_pass_kawase_draw_inline(kawase_pass, effect->uniform.bloom.blur,
                                    command_encoder);
  }
  profiler_latency_end(fx->profiler, ProfilerLatencyType_BloomPass);
}

/**

                                       .- attachment view -----.
   .---------.                        |          _____         |
   |  view   | ----.   .----------.   |       __|____|__       |
   '---------'     |__| bindgroup |-> |      |  .---.   |      |
   .---------.     |  '-----------'   |      --;____;---'      |
   | uniform | ----'                  |                        |
   '---------'                        '-----------------------'


   We load resources bound to bindgroup (view, uniform)
   Read and compute them through shader
   Print the output to the attachment view
 */

static inline void post_fx_composite_draw(PostFx *fx,
                                          WGPUCommandEncoder command_encoder) {

  WGPURenderPassColorAttachment color_attachment = {
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
      .view = wgpuSwapChainGetCurrentTextureView(context_swapchain()),
      .loadOp = WGPULoadOp_Load,
      .storeOp = WGPUStoreOp_Store,
  };

  WGPURenderPassDescriptor pass_desc = {
      .label = "Composite Resolve Pass",
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
  };

  profiler_latency_start(fx->profiler, ProfilerLatencyType_CompositePass);
  {
    WGPURenderPassEncoder pass =
        wgpuCommandEncoderBeginRenderPass(command_encoder, &pass_desc);
    {
      // POST F
      wgpuRenderPassEncoderSetPipeline(
          pass, (*post_fx_effect(fx, PostFxType_Composite)->pipeline)->handle);
      wgpuRenderPassEncoderSetBindGroup(
          pass, 0, post_fx_effect(fx, PostFxType_Composite)->bindgroup, 0,
          NULL);
      wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
    }

    wgpuRenderPassEncoderEnd(pass);
  }
  profiler_latency_end(fx->profiler, ProfilerLatencyType_CompositePass);
}

#endif
