#ifndef _SHADOW_MAP_CORE_H_
#define _SHADOW_MAP_CORE_H_

#include "../runtime/mesh/mesh.h"
#include "../runtime/light/light.h"

#include <webgpu/webgpu.h>

#define SHADOW_DEPTH_FORMAT WGPUTextureFormat_Depth32Float
#define SHADOW_COLOR_FORMAT WGPUTextureFormat_BGRA8Unorm
#define SHADOW_MAP_SIZE 1024

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  MeshRefList *mesh_list;
  struct {
    PointLightList *point;
    SpotLightList *spot;
    SunLightList *sun;
  } lights;
} ShadowMapInitDescriptor;


typedef struct {
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;
  const WGPUQueue queue;
} ShadowPassFallbackToTextureDescriptor;


void shadow_pass_init(const ShadowMapInitDescriptor *);

#endif
