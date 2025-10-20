#include "core.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/std_pipeline/core.h"
#include "backend/std_pipeline/render_shader/bloom/bloom.h"
#include "backend/std_pipeline/render_shader/composite/composite.h"
#include "runtime/pipeline/render.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

static inline PostFxStatus post_fx_validate_create(PostFx *, const PostFxType);
static inline PostFxStatus post_fx_bloom_destroy(PostFx *);
static inline PostFxStatus post_fx_add_callback(PostFx *,
                                                post_fx_draw_callback);

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

  fx->callbacks.length = 0;

  return PostFxStatus_Success;
}

PostFxStatus post_fx_destroy(PostFx *fx) {
  wgpuSamplerRelease(fx->sampler);
  post_fx_bloom_destroy(fx);

  return PostFxStatus_Success;
}

PostFxStatus post_fx_validate_create(PostFx *fx, const PostFxType type) {

  static const char *label[] = {
      [PostFxType_Blit] = "Blit",
      [PostFxType_Bloom] = "Bloom",
      [PostFxType_Composite] = "Composite",
  };

  if (fx->state & type) {
    logger_add(LoggerFlag_Error,
               "Cannot initialize post fx %s, already created. Make sure to "
               "destroy the effect beffore creating it.",
               label[type]);
    return PostFxStatus_AlreadyCreated;
  }

  // update state
  fx->state |= type;

  return PostFxStatus_Success;
}

PostFxStatus post_fx_update_effect_view(PostFx *fx, const PostFxType type,
                                        const uint8_t index,
                                        const WGPUTextureView view) {

  if (__builtin_ctz(type) > POST_FX_TYPE_COUNT) {
    logger_add(LoggerFlag_Error,
               "Attempting to update view of an unknown post fx type (%d)",
               type);
    return PostFxStatus_UnknownType;
  } else if (index >= POST_FX_MAX_BUFFER) {
    logger_add(LoggerFlag_Error,
               "Attempting to update a post fx out of bound view (%d), "
               "maximum views allowed: %d",
               index, POST_FX_MAX_BUFFER);
    return PostFxStatus_MaxCapacity;
  }

  PostFxEffect *effect = post_fx_effect(fx, type);
  effect->view[index] = view;

  if (effect->bindgroup_creator)
    effect->bindgroup_creator(fx);

  return PostFxStatus_Success;
}

/**
   Overral effect process:
   1. validate if not already created
   2. define core attributes: pipeline, view(s), uniforms
   3. create bindgroup
   4. update fx callbacks and length (i.e. subscribe to draw loop)
 */
PostFxStatus post_fx_blit_create(PostFx *fx, const WGPUTextureView view) {

  const PostFxType fx_type = PostFxType_Blit;
  const RenderPipelineType pipeline_type = RenderPipelineType_Blit;

  // === Validate ===
  {
    PostFxStatus validate_status = post_fx_validate_create(fx, fx_type);
    if (validate_status != PostFxStatus_Success)
      return validate_status;
  }

  PostFxEffect *effect = post_fx_effect(fx, fx_type);

  // === Define core attributes ===
  {
    effect->pipeline = std_render_pipeline(pipeline_type);
    effect->view[POST_FX_VIEW_INDEX_SCENE] = view;
    effect->texture = NULL;
    effect->bindgroup_creator = post_fx_blit_create_bindgroup;
  }

  effect->bindgroup_creator(fx);
  post_fx_add_callback(fx, post_fx_blit_draw);

  return PostFxStatus_Success;
}

PostFxStatus post_fx_bloom_create(PostFx *fx, const WGPUTextureView view,
                                  const BloomUniform uniform, const int width,
                                  const int height) {

  const PostFxType fx_type = PostFxType_Bloom;
  const RenderPipelineType pipeline_type = RenderPipelineType_Bloom;

  // === Validate ===
  {
    PostFxStatus validate_status = post_fx_validate_create(fx, fx_type);
    if (validate_status != PostFxStatus_Success)
      return validate_status;
  }

  PostFxEffect *effect = post_fx_effect(fx, fx_type);

  // === Define core attributes ===
  {
    effect->pipeline = std_render_pipeline(pipeline_type);
    effect->view[POST_FX_VIEW_INDEX_SCENE] = view;
    effect->bindgroup_creator = post_fx_bloom_create_bindgroup;

    effect->texture = wgpuDeviceCreateTexture(
        context_device(), &(WGPUTextureDescriptor){
                              .label = "Bloom texture",
                              .dimension = WGPUTextureDimension_2D,
                              .format = TEXTURE_FORMAT_OFFSCREEN,
                              .usage = WGPUTextureUsage_TextureBinding |
                                       WGPUTextureUsage_RenderAttachment,
                              .sampleCount = 1,
                              .mipLevelCount = 1,
                              .size =
                                  (WGPUExtent3D){
                                      .height = height,
                                      .width = width,
                                      .depthOrArrayLayers = 1,
                                  },
                          });
    effect->view[POST_FX_VIEW_INDEX_BLOOM] =
        wgpuTextureCreateView(effect->texture, NULL);

    effect->uniform.bloom = uniform;
  }

  effect->bindgroup_creator(fx);
  post_fx_add_callback(fx, post_fx_bloom_draw);

  return PostFxStatus_Success;
}

PostFxStatus post_fx_create_composite(PostFx *fx, const WGPUTextureView view,
                                      const CompositeUniform uniform) {

  const PostFxType fx_type = PostFxType_Composite;
  const RenderPipelineType pipeline_type = RenderPipelineType_Composite;

  // === Validate ===
  {
    PostFxStatus validate_status = post_fx_validate_create(fx, fx_type);
    if (validate_status != PostFxStatus_Success)
      return validate_status;

    // requires Post Fx Bloom to be setup (TODO: will be replace by fallback
    // texture in future)
    if (post_fx_effect(fx, PostFxType_Bloom)->view[POST_FX_VIEW_INDEX_BLOOM] ==
        NULL) {
      logger_add(LoggerFlag_Error,
                 "Composite effect require bloom post fx to be created");
      return PostFxStatus_MissingNecessaryResource;
    }
  }

  PostFxEffect *effect = post_fx_effect(fx, fx_type);

  // === Define core attributes ===
  {
    effect->pipeline = std_render_pipeline(pipeline_type);
    effect->view[POST_FX_VIEW_INDEX_SCENE] = view;
    effect->view[POST_FX_VIEW_INDEX_BLOOM] =
        post_fx_effect(fx, PostFxType_Bloom)->view[POST_FX_VIEW_INDEX_BLOOM];
    effect->bindgroup_creator = post_fx_composite_create_bindgroup;
    effect->uniform.composite = uniform;
  }

  effect->bindgroup_creator(fx);
  post_fx_add_callback(fx, post_fx_composite_draw);

  return PostFxStatus_Success;
}

/**
   Bloom uses a special independent texture, so need to clear it on post-fx
   destroy
 */
PostFxStatus post_fx_bloom_destroy(PostFx *fx) {

  wgpuTextureRelease(post_fx_effect(fx, PostFxType_Bloom)->texture);
  wgpuTextureViewRelease(post_fx_effect(fx, PostFxType_Bloom)->view[0]);

  return PostFxStatus_Success;
}

PostFxStatus post_fx_blit_create_bindgroup(PostFx *fx) {

  PostFxEffect *effect = post_fx_effect(fx, PostFxType_Blit);

  // clean up
  if (effect->bindgroup)
    wgpuBindGroupRelease(effect->bindgroup);

  const WGPURenderPipeline pipeline = effect->pipeline->handle;
  const WGPUBindGroupLayout bind_group_layout =
      wgpuRenderPipelineGetBindGroupLayout(pipeline, 0);

  WGPUBindGroupEntry entries[2] = {
      {.binding = 0, .textureView = effect->view[POST_FX_VIEW_INDEX_SCENE]},
      {.binding = 1, .sampler = fx->sampler},
  };
  WGPUBindGroupDescriptor bg_desc = {
      .layout = bind_group_layout,
      .entryCount = 2,
      .entries = entries,
  };

  effect->bindgroup = wgpuDeviceCreateBindGroup(context_device(), &bg_desc);
  return PostFxStatus_Success;
}

PostFxStatus post_fx_bloom_create_bindgroup(PostFx *fx) {

  PostFxEffect *effect = post_fx_effect(fx, PostFxType_Bloom);

  if (effect->bindgroup)
    wgpuBindGroupRelease(effect->bindgroup);

  const WGPURenderPipeline pipeline = effect->pipeline->handle;
  const WGPUBindGroupLayout bind_group_layout =
      wgpuRenderPipelineGetBindGroupLayout(pipeline, 0);

  if (effect->buffer[0] == NULL)
    effect->buffer[0] = wgpuDeviceCreateBuffer(
        context_device(), &(WGPUBufferDescriptor){
                              .label = "Bloom buffer",
                              .size = sizeof(BloomUniform),
                              .usage = WGPUBufferUsage_Uniform,
                          });

  wgpuQueueWriteBuffer(context_queue(), effect->buffer[0], 0,
                       &effect->uniform.bloom, sizeof(BloomUniform));

  WGPUBindGroupEntry entries[3] = {
      {.binding = 0, .textureView = effect->view[POST_FX_VIEW_INDEX_SCENE]},
      {.binding = 1, .sampler = fx->sampler},
      {
          .binding = 2,
          .buffer = effect->buffer[0],
          .size = sizeof(BloomUniform),
      },
  };

  WGPUBindGroupDescriptor bg_desc = {
      .layout = bind_group_layout,
      .entryCount = 3,
      .entries = entries,
  };

  effect->bindgroup = wgpuDeviceCreateBindGroup(context_device(), &bg_desc);
  return PostFxStatus_Success;
}

PostFxStatus post_fx_composite_create_bindgroup(PostFx *fx) {

  PostFxEffect *effect = post_fx_effect(fx, PostFxType_Composite);

  if (effect->bindgroup)
    wgpuBindGroupRelease(effect->bindgroup);

  const WGPURenderPipeline pipeline = effect->pipeline->handle;
  const WGPUBindGroupLayout bind_group_layout =
      wgpuRenderPipelineGetBindGroupLayout(pipeline, 0);

  if (effect->buffer[0] == NULL)
    effect->buffer[0] = wgpuDeviceCreateBuffer(
        context_device(), &(WGPUBufferDescriptor){
                              .label = "Composite Buffer",
                              .size = sizeof(CompositeUniform),
                              .usage = WGPUTextureUsage_TextureBinding,
                          });

  wgpuQueueWriteBuffer(context_queue(), effect->buffer[0], 0,
                       &effect->uniform.composite, sizeof(CompositeUniform));

  WGPUBindGroupEntry entries[4] = {
      {.binding = 0, .textureView = effect->view[POST_FX_VIEW_INDEX_SCENE]},
      {.binding = 1, .textureView = effect->view[POST_FX_VIEW_INDEX_BLOOM]},
      {.binding = 2, .sampler = fx->sampler},
      {
          .binding = 3,
          .buffer = effect->buffer[0],
          .size = sizeof(CompositeUniform),
      },
  };

  WGPUBindGroupDescriptor bg_desc = {
      .layout = bind_group_layout,
      .entryCount = 4,
      .entries = entries,
  };

  effect->bindgroup = wgpuDeviceCreateBindGroup(context_device(), &bg_desc);

  return PostFxStatus_Success;
}

PostFxStatus post_fx_add_callback(PostFx *fx, post_fx_draw_callback callback) {

  if (fx->callbacks.length == POST_FX_TYPE_COUNT) {
    logger_add(LoggerFlag_Error, "Post fx callback list reached max capacity.");
    return PostFxStatus_MaxCapacity;
  }

  fx->callbacks.entries[fx->callbacks.length++] = callback;

  return PostFxStatus_Success;
}
