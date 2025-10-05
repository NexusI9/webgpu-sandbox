#ifndef _POST_FX_CORE_H_
#define _POST_FX_CORE_H_

#include "runtime/pipeline/render.h"
#include <webgpu/webgpu.h>

#define POST_FX_BINDGROUP_CACHE_CAPACITY 6
#define POST_FX_TYPE_COUNT 1

typedef enum {
  PostFxStatus_Success,
  PostFxStatus_MaxCapacity,
  PostFxStatus_ViewUnfound,
  PostFxStatus_UndefError,

} PostFxStatus;

typedef enum {
  PostFxType_Blit,
} PostFxType;

typedef struct {
  WGPUTextureView view;
  WGPUBindGroup bindgroup;
} PostFxBindgroup;

typedef struct {
  PostFxBindgroup entries[POST_FX_BINDGROUP_CACHE_CAPACITY];
  size_t length;
} PostFxBindgroupList;

typedef struct {
  WGPUSampler sampler;
  PostFxBindgroupList bingroup_list[POST_FX_TYPE_COUNT];
} PostFx;

typedef struct {

} PostFxDescriptor;

PostFxStatus post_fx_init(PostFx *, const PostFxDescriptor *);
PostFxStatus post_fx_destroy(PostFx *);
PostFxStatus post_fx_bind_texture_view(PostFx *, const PostFxType,
                                       const WGPUTextureView);

PostFxStatus post_fx_update_bindgroup_view(PostFx *, const PostFxType,
                                           const WGPUTextureView,
                                           const WGPUTextureView);

void post_fx_blit(PostFx *, WGPUTextureView, WGPURenderPassEncoder);

#endif
