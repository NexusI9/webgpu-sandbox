#ifndef _POST_FX_CORE_H_
#define _POST_FX_CORE_H_

#include "backend/compute/core.h"
#include "backend/compute/kawase.h"
#include "backend/context.h"
#include "backend/std_pipeline/render_shader/bloom/bloom.h"
#include "backend/std_pipeline/render_shader/composite/composite.h"
#include "runtime/pipeline/render.h"
#include <stdint.h>
#include <webgpu/webgpu.h>

typedef struct PostFx PostFx;

typedef enum {
  PostFxStatus_Success,
  PostFxStatus_MaxCapacity,
  PostFxStatus_AlreadyCreated,
  PostFxStatus_MissingNecessaryResource,
  PostFxStatus_UnknownType,
  PostFxStatus_SameAttribute,
  PostFxStatus_UndefError,
} PostFxStatus;

#define POST_FX_TYPE_COUNT 3
typedef enum {
  PostFxType_Blit = 1 << 0,
  PostFxType_Bloom = 1 << 1,
  PostFxType_Composite = 1 << 2,
} PostFxType;

typedef enum {
  PostFxViewIndex_Scene,
  PostFxViewIndex_Bloom,
} PostFxViewIndex;

/**
   Overall flow:

     Create phase            Optimized Cache              Draw
      (Storage)              (DOD friendly)          (linearly read)

   .-------------.
   | Bloom       |
   | + texture   |
   | + view      |----.   .- array --------.
   | + uniform   |    |   |  Bind group    |       .----------------.
   '-------------'    |   |  View          |      |                  ▼
   .-------------.    '-->|  Pipeline      |         set pipeline
   | Composite   |        |----------------| ---->   set bind group
   | + texture   |        | Bind group     |         pass draw
   | + view      |------->| View           |      ▲                 |
   | + uniform   |        | Pipeline       |      '-----------------'
   '-------------'        |----------------|
   .-------------.        | Bind group     |
   | Blit        |   .--->| View           |
   | + texture   |---'    | Pipeline       |
   | + view      |        '----------------'
   '-------------'

   Post effects follow a certain order and are co-dependent, as instance, there
   would be no interest ot have first call the composite effect and then de
   bloom effect.

 */

typedef PostFxStatus (*post_fx_bindgroup_creator)(PostFx *);
typedef void (*post_fx_draw_callback)(PostFx *, WGPUCommandEncoder);

#define POST_FX_MAX_BUFFER 6
typedef struct {

  // cached bindgroup created with the effect parameter (view, unfiroms...)
  WGPUBindGroup bindgroup;
  const RenderPipeline *pipeline;

  // effect view are either created from texture above of directly shared from
  // an external source
  WGPUTextureView view[POST_FX_MAX_BUFFER];
  WGPUBuffer buffer[POST_FX_MAX_BUFFER];
  // optional texture if we want the post fx to use a independent texture
  WGPUTexture texture;
  post_fx_bindgroup_creator bindgroup_creator;

  union {
    CompositeUniform composite;
    BloomUniform bloom;
  } uniform;

} PostFxEffect;

struct PostFx {

  PostFxType state;
  WGPUSampler sampler; // common sampler used in each effect
  PostFxEffect effects[POST_FX_TYPE_COUNT];
  ComputePass *compute; // scene renderer compute pass (used to blur the bloom)

  struct {
    post_fx_draw_callback entries[POST_FX_TYPE_COUNT];
    uint8_t length;
  } callbacks;
};

typedef struct {
  ComputePass *compute;
} PostFxDescriptor;

PostFxStatus post_fx_init(PostFx *, const PostFxDescriptor *);
PostFxStatus post_fx_destroy(PostFx *);

// Creators
PostFxStatus post_fx_blit_create(PostFx *, const WGPUTextureView);

PostFxStatus post_fx_bloom_create(PostFx *, const WGPUTextureView,
                                  const BloomUniform, const int width,
                                  const int height);

PostFxStatus post_fx_composite_create(PostFx *, const WGPUTextureView,
                                      const CompositeUniform);

PostFxStatus post_fx_update_effect_view(PostFx *, const PostFxType,
                                        const PostFxViewIndex,
                                        const WGPUTextureView);

static inline PostFxEffect *post_fx_effect(PostFx *fx, const PostFxType type) {
  return &fx->effects[__builtin_ctz(type)];
}

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
        pass, post_fx_effect(fx, PostFxType_Blit)->pipeline->handle);
    wgpuRenderPassEncoderSetBindGroup(
        pass, 0, post_fx_effect(fx, PostFxType_Blit)->bindgroup, 0, NULL);
    wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
  }

  wgpuRenderPassEncoderEnd(pass);
}

static inline void post_fx_bloom_draw(PostFx *fx,
                                      WGPUCommandEncoder command_encoder) {

  const PostFxEffect *effect = post_fx_effect(fx, PostFxType_Bloom);
  WGPURenderPassColorAttachment color_attachment = {
      .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
      .view = effect->view[PostFxViewIndex_Bloom],
      .loadOp = WGPULoadOp_Load,
      .storeOp = WGPUStoreOp_Store,
  };

  WGPURenderPassDescriptor pass_desc = {
      .label = "Bloom Resolve Pass",
      .colorAttachmentCount = 1,
      .colorAttachments = &color_attachment,
  };

  WGPURenderPassEncoder pass =
      wgpuCommandEncoderBeginRenderPass(command_encoder, &pass_desc);
  {
    // POST FX
    wgpuRenderPassEncoderSetPipeline(pass, effect->pipeline->handle);
    wgpuRenderPassEncoderSetBindGroup(pass, 0, effect->bindgroup, 0, NULL);
    wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
  }

  wgpuRenderPassEncoderEnd(pass);

  KawaseDescriptor blur_desc = {
      .texture = effect->texture,
      .layer_count = 1,
      .pass_count = effect->uniform.bloom.blur,
  };
  compute_pass_kawase_inline(fx->compute, &blur_desc, command_encoder);
}

/**

                                       .- attachment view -----.
   .---------.                        |           _____        |
   |  view   | ----.   .----------.   |        __|____|__      |
   '---------'     |__| bindgroup |-> |       |  .---.   |     |
   .---------.     |  '-----------'   |       --;____;---'     |
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

  // RESOLVE (1x) ==> POSTFX ==> SWAPCHAIN
  WGPURenderPassEncoder pass =
      wgpuCommandEncoderBeginRenderPass(command_encoder, &pass_desc);
  {
    // POST FX
    wgpuRenderPassEncoderSetPipeline(
        pass, post_fx_effect(fx, PostFxType_Composite)->pipeline->handle);
    wgpuRenderPassEncoderSetBindGroup(
        pass, 0, post_fx_effect(fx, PostFxType_Composite)->bindgroup, 0, NULL);
    wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
  }

  wgpuRenderPassEncoderEnd(pass);
}

static inline void post_fx_draw(PostFx *fx,
                                WGPUCommandEncoder command_encoder) {
  // loop through subscribed effects
  for (uint8_t i = 0; i < fx->callbacks.length; i++)
    fx->callbacks.entries[i](fx, command_encoder);
}

PostFxStatus post_fx_blit_create_bindgroup(PostFx *);
PostFxStatus post_fx_bloom_create_bindgroup(PostFx *);
PostFxStatus post_fx_composite_create_bindgroup(PostFx *);

// Update

PostFxStatus post_fx_bloom_update_texture_resolution(PostFx *, const int,
                                                     const int);

static inline PostFxStatus
post_fx_bloom_update_uniform(PostFx *fx, const BloomUniform uniform) {
  PostFxEffect *effect = post_fx_effect(fx, PostFxType_Bloom);
  effect->uniform.bloom = uniform;
  wgpuQueueWriteBuffer(context_queue(), effect->buffer[0], 0,
                       &effect->uniform.bloom, sizeof(BloomUniform));
  return PostFxStatus_Success;
}

static inline PostFxStatus
post_fx_composite_update_uniform(PostFx *fx, const CompositeUniform uniform) {
  PostFxEffect *effect = post_fx_effect(fx, PostFxType_Composite);
  effect->uniform.composite = uniform;
  wgpuQueueWriteBuffer(context_queue(), effect->buffer[0], 0,
                       &effect->uniform.composite, sizeof(CompositeUniform));
  return PostFxStatus_Success;
}

#endif
