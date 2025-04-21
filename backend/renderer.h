#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../runtime/scene.h"
#include "clock.h"
#include "webgpu/webgpu.h"

// WGPUTextureFormat_Depth32Float
#define SHADOW_DEPTH_FORMAT WGPUTextureFormat_Depth32Float
#define SHADOW_COLOR_FORMAT WGPUTextureFormat_BGRA8Unorm
#define SHADOW_MAP_SIZE 1024

typedef struct {
  const char *name;
  cclock *clock;
  bool lock_mouse;
} RendererCreateDescriptor;

typedef struct renderer {

  cclock *clock; // update clock delta on draw

  struct {
    const char *name;
    int width;
    int height;
    int dpi;
  } context;

  struct {
    WGPUInstance instance;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSwapChain swapchain;
    WGPURenderPipeline pipeline;
  } wgpu;

} renderer;

typedef struct {
  WGPUDevice device;
  WGPUQueue queue;
  scene *scene;

  struct {
    WGPUTexture *color_texture;
    WGPUTexture *depth_texture;
  } point_light;

  struct {
    WGPUTexture *color_texture;
    WGPUTexture *depth_texture;
  } directional_light;

} RendererCreateShadowMapDescriptor;

typedef struct {
  scene *scene;
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;
  const WGPUDevice *device;
  WGPUCommandEncoder encoder;
} RendererShadowToTextureDescriptor;

typedef struct {
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;
  const WGPUQueue queue;
} RendererShadowFallbackToTextureDescriptor;

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
} RendererCreateShadowTextureDescriptor;

renderer renderer_create(const RendererCreateDescriptor *);
void renderer_init(renderer *);
void renderer_draw(const renderer *, scene *);
void renderer_compute_shadow(renderer *, scene *);

void renderer_end_frame(const renderer *);
void renderer_set_draw(const void *);
void renderer_lock_mouse(const renderer *);

#endif
