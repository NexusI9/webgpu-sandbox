#ifndef _SHADOW_MAP_TEXTURE_H_
#define _SHADOW_MAP_TEXTURE_H_

#include <webgpu/webgpu.h>

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

void shadow_pass_texture_create(const ShadowPassTextureDescriptor *);

#endif
