#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../runtime/scene.h"
#include "clock.h"
#include "webgpu/webgpu.h"

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

renderer renderer_create(const RendererCreateDescriptor *);
void renderer_init(renderer *);
void renderer_draw(const renderer *, scene *);
void renderer_bake_ao(renderer *, scene *);
void renderer_compute_shadow(renderer *, scene *);

void renderer_end_frame(const renderer *);
void renderer_set_draw(const void *);
void renderer_lock_mouse(const renderer *);

#endif
