#ifndef _SHADOW_PASS_H_
#define _SHADOW_PASS_H_

#include "../runtime/scene/scene.h"
#include <webgpu/webgpu.h>

#define SHADOW_DEPTH_FORMAT WGPUTextureFormat_Depth32Float
#define SHADOW_COLOR_FORMAT WGPUTextureFormat_BGRA8Unorm
#define SHADOW_MAP_SIZE 1024

typedef struct {
  WGPUDevice device;
  WGPUQueue queue;
  Scene *scene;

  struct {
    WGPUTexture *color_texture;
    WGPUTexture *depth_texture;
  } point_light;

  struct {
    WGPUTexture *color_texture;
    WGPUTexture *depth_texture;
  } directional_light;

} ShadowPassMapDescriptor;

typedef struct {
  Scene *scene;
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;
  const WGPUDevice *device;
  WGPUCommandEncoder encoder;
} ShadowPassToTextureDescriptor;

typedef struct {
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;
  const WGPUQueue queue;
} ShadowPassFallbackToTextureDescriptor;

typedef struct {

  struct {
    WGPUTexture *texture;
    WGPUTextureView *texture_view;
  } color;

  struct {
    WGPUTexture *texture;
    WGPUTextureView *texture_view;
  } depth;

  int width;
  int height;
  size_t layer_count;
  WGPUDevice device;
  WGPUTextureViewDimension dimension;
} ShadowPassTextureDescriptor;

void shadow_pass_init(Scene *, WGPUDevice, WGPUQueue);

#endif
