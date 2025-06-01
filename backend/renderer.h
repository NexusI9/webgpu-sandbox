#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../runtime/scene/scene.h"
#include "clock.h"
#include "webgpu/webgpu.h"

typedef struct {
  const char *name;
  cclock *clock;
  bool lock_mouse;
} RendererCreateDescriptor;

typedef struct {
  struct renderer *renderer;
  Scene *scene;
  scene_draw_callback draw_callback;
} RendererRenderDescriptor;

typedef enum {
  RendererDrawMode_Texture,
  RendererDrawMode_Solid,
  RendererDrawMode_Wireframe,
} RendererDrawMode;

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

void renderer_create(renderer *, const RendererCreateDescriptor *);
void renderer_init(renderer *);
void renderer_bake_ao(renderer *, Scene *);
void renderer_compute_shadow(renderer *, Scene *);

void renderer_close(const renderer *);
void renderer_draw(renderer *, Scene *, const RendererDrawMode);
void renderer_lock_mouse(const renderer *);

WGPUDevice *renderer_device(renderer *);
WGPUQueue *renderer_queue(renderer *);
#endif
