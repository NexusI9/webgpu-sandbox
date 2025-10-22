#ifndef _SHADOW_MAP_CORE_H_
#define _SHADOW_MAP_CORE_H_

#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "runtime/texture/core.h"

#define SHADOW_DEPTH_FORMAT WGPUTextureFormat_Depth32Float
#define SHADOW_COLOR_FORMAT TEXTURE_FORMAT_ONSCREEN
#define SHADOW_MAP_SIZE 512

typedef struct {

  const RenderPassDrawListDescriptor *draw_list;
  LightList *lights;
} ShadowMapInitDescriptor;

typedef struct {
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;

} ShadowPassFallbackToTextureDescriptor;

typedef struct {
  const int width;
  const int height;
  const size_t layer_count;
  const WGPUTextureViewDimension dimension;
  WGPUTexture *color_texture, *depth_texture;
  WGPUTextureView *color_view, *depth_view;
} ShadowPassTextureDescriptor;

#ifdef __cplusplus
extern "C" {
#endif

void shadow_map_init(const ShadowMapInitDescriptor *);

void shadow_map_pass_preprocessor_callback(const RenderPass *, Mesh *, void *);

void shadow_pass_texture_create(const ShadowPassTextureDescriptor *);

void shadow_pass_update_resolution(RenderPass *, const TextureResolution,
                                   const WGPUTextureViewDimension);

#ifdef __cplusplus
}
#endif

#endif
