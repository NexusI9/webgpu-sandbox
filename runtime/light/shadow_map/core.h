#ifndef _SHADOW_MAP_CORE_H_
#define _SHADOW_MAP_CORE_H_

#include "../runtime/light/list.h"

#include <webgpu/webgpu.h>

#define SHADOW_DEPTH_FORMAT WGPUTextureFormat_Depth32Float
#define SHADOW_COLOR_FORMAT TEXTURE_FORMAT_ONSCREEN_DEFAULT
#define SHADOW_MAP_SIZE 512

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const RenderPassDrawListDescriptor *draw_list;
  LightList *lights;
} ShadowMapInitDescriptor;

typedef struct {
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;
  const WGPUQueue queue;
} ShadowPassFallbackToTextureDescriptor;

typedef struct {
  const int width;
  const int height;
  const size_t layer_count;
  const WGPUDevice device;
  const WGPUQueue queue;
  const WGPUTextureViewDimension dimension;
  RenderPass *pass;
  const RenderPassDrawListDescriptor *draw_list;
} ShadowPassTextureDescriptor;


void shadow_map_init(const ShadowMapInitDescriptor *);

void shadow_map_pass_preprocessor_callback(const RenderPass *, Mesh *, void *);

void shadow_pass_texture_create(const ShadowPassTextureDescriptor *);

#endif
