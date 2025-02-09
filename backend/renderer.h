#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../runtime/scene.h"

typedef struct {

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

renderer renderer_create(const char *);
void renderer_init(renderer *);
void renderer_draw(const renderer *, scene *);

void renderer_end_frame(const renderer *);

#endif
