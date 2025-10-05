#include "core.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/std_pipeline/core.h"
#include "runtime/pipeline/render.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

typedef WGPUBindGroup (*post_fx_type_bind_group_create)(PostFx *,
                                                        WGPUTextureView);

static inline WGPUBindGroup
post_fx_blit_bind_group_create(PostFx *fx, WGPUTextureView view);

PostFxStatus post_fx_init(PostFx *fx, const PostFxDescriptor *desc) {

  fx->sampler = wgpuDeviceCreateSampler(
      context_device(), &(WGPUSamplerDescriptor){
                            .label = "PostFX Common Sampler",
                            .addressModeU = WGPUAddressMode_ClampToEdge,
                            .addressModeV = WGPUAddressMode_ClampToEdge,
                            .addressModeW = WGPUAddressMode_ClampToEdge,
                            .magFilter = WGPUFilterMode_Linear,
                            .minFilter = WGPUFilterMode_Linear,
                            .mipmapFilter = WGPUMipmapFilterMode_Linear,
                        });

  return PostFxStatus_Success;
}

PostFxStatus post_fx_destroy(PostFx *fx) {
  wgpuSamplerRelease(fx->sampler);
  return PostFxStatus_Success;
}

WGPUBindGroup post_fx_blit_bind_group_create(PostFx *fx, WGPUTextureView view) {

  const WGPURenderPipeline blit_pipeline =
      std_render_pipeline(RenderPipelineType_Blit)->handle;

  const WGPUBindGroupLayout bind_group_layout =
      wgpuRenderPipelineGetBindGroupLayout(blit_pipeline, 0);

  // Bind your offscreen texture + sampler
  WGPUBindGroupEntry entries[2] = {
      {.binding = 0, .textureView = view},
      {.binding = 1, .sampler = fx->sampler},
  };
  WGPUBindGroupDescriptor bg_desc = {
      .layout = bind_group_layout,
      .entryCount = 2,
      .entries = entries,
  };

  return wgpuDeviceCreateBindGroup(context_device(), &bg_desc);
}

static const post_fx_type_bind_group_create bind_creator[POST_FX_TYPE_COUNT] = {
    [PostFxType_Blit] = post_fx_blit_bind_group_create,
};
PostFxStatus post_fx_bind_texture_view(PostFx *fx, const PostFxType type,
                                       const WGPUTextureView view) {

  PostFxBindgroupList *list = &fx->bingroup_list[type];

  if (list->length == POST_FX_BINDGROUP_CACHE_CAPACITY) {
    logger_add(LoggerFlag_Error, "PostFx bindgroup reached max capacity.");
    return PostFxStatus_MaxCapacity;
  }

  list->entries[list->length].bindgroup = bind_creator[type](fx, view);
  list->entries[list->length].view = view;
  list->length++;

  return PostFxStatus_Success;
}

/**
   Update the view of the cached bind group, meaning we also need to rebuild the
   bindgroup.
 */

PostFxStatus post_fx_update_bindgroup_view(PostFx *fx, const PostFxType type,
                                           const WGPUTextureView old,
                                           const WGPUTextureView new) {

  int found = 0;
  for (size_t i = 0; i < fx->bingroup_list[type].length; i++) {

    PostFxBindgroup *bg = &fx->bingroup_list[type].entries[i];

    if (bg->view == old) {
      bg->view = new;

      wgpuBindGroupRelease(bg->bindgroup);
      bg->bindgroup = bind_creator[type](fx, new);

      found++;
    }
  }

  return found ? PostFxStatus_Success : PostFxStatus_ViewUnfound;
}

void post_fx_blit(PostFx *fx, WGPUTextureView view,
                  WGPURenderPassEncoder pass) {

  const WGPURenderPipeline blit_pipeline =
      std_render_pipeline(RenderPipelineType_Blit)->handle;

  // lookup cached bindgroup
  for (uint8_t i = 0; i < fx->bingroup_list[PostFxType_Blit].length; i++) {
    if (fx->bingroup_list[PostFxType_Blit].entries[i].view == view) {
      wgpuRenderPassEncoderSetPipeline(pass, blit_pipeline);
      wgpuRenderPassEncoderSetBindGroup(
          pass, 0, fx->bingroup_list[PostFxType_Blit].entries[i].bindgroup, 0,
          NULL);
      wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
      break;
    }
  }
}
