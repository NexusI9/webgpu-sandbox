#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../runtime/scene/scene.h"
#include "clock.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

#define RENDERER_DPI_AUTO 0

typedef struct {
  const char *name;
  cclock *clock;
  PipelineMultisampleCount multisampling_count;
  WGPUColor background;
  double dpi;
} RendererCreateDescriptor;

typedef enum {
  RendererDrawMode_Texture,
  RendererDrawMode_Solid,
  RendererDrawMode_Wireframe,
  RendererDrawMode_Boundbox,
} RendererDrawMode;

typedef struct Renderer {

  cclock *clock; // update clock delta on draw
  WGPUColor background;

  struct {
    const char *name;
    int width;
    int height;
    double dpi;
  } context;

  struct {
    WGPUInstance instance;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSwapChain swapchain;
    WGPURenderPipeline pipeline;
  } wgpu;

  struct {
    PipelineMultisampleCount count;
    WGPUTextureView view;
  } multisampling;

  struct {
    WGPUTextureView view;
  } depth;

} Renderer;

typedef WGPURenderPassColorAttachment (*renderer_color_attachment_callback)(
    Renderer *, WGPUTextureView);

typedef struct {
  Renderer *renderer;
  Scene *scene;
  renderer_color_attachment_callback color_attachment_callback;

  struct {
    scene_draw_callback *entries;
    size_t length;
  } draw_list;

} RendererRenderDescriptor;

void renderer_create(Renderer *, const RendererCreateDescriptor *);

void renderer_bake_ao(Renderer *, Scene *);
void renderer_compute_shadow(Renderer *, Scene *);

void renderer_close(const Renderer *);
void renderer_draw(Renderer *, Scene *, const RendererDrawMode);

WGPUDevice *renderer_device(Renderer *);
WGPUQueue *renderer_queue(Renderer *);
int renderer_width(Renderer *);
int renderer_height(Renderer *);

const char *renderer_target(Renderer *);
#endif
