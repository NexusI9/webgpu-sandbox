#ifndef _SHADOW_MAP_CORE_H_
#define _SHADOW_MAP_CORE_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"

#include <webgpu/webgpu.h>

#define SHADOW_DEPTH_FORMAT WGPUTextureFormat_Depth32Float
#define SHADOW_COLOR_FORMAT WGPUTextureFormat_BGRA8Unorm
#define SHADOW_MAP_SIZE 1024

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  LightList *lights;
} ShadowMapInitDescriptor;

typedef struct {
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;
  const WGPUQueue queue;
} ShadowPassFallbackToTextureDescriptor;

void shadow_map_init(const ShadowMapInitDescriptor *);

#endif
