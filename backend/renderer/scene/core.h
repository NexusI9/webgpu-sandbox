#ifndef _SCENE_RENDERER_CORE_H_
#define _SCENE_RENDERER_CORE_H_

#include "../runtime/scene/scene.h"
#include "../../clock.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

#define RENDERER_DPI_AUTO 0

typedef struct {
  const char *name;
  cclock *clock;
  PipelineMultisampleCount multisampling_count;
  WGPUColor background;
  double dpi;
} SceneRendererCreateDescriptor;

typedef enum {
  SceneRendererDrawMode_Texture,
  SceneRendererDrawMode_Solid,
  SceneRendererDrawMode_Wireframe,
  SceneRendererDrawMode_Boundbox,
} SceneRendererDrawMode;

typedef struct SceneRenderer {

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

} SceneRenderer;

typedef WGPURenderPassColorAttachment (*renderer_color_attachment_callback)(
    SceneRenderer *, WGPUTextureView);

typedef struct {
  SceneRenderer *renderer;
  Scene *scene;
  renderer_color_attachment_callback color_attachment_callback;

  struct {
    scene_draw_callback *entries;
    size_t length;
  } draw_list;

} SceneRendererRenderDescriptor;

void renderer_create(SceneRenderer *, const SceneRendererCreateDescriptor *);

void renderer_bake_ao(SceneRenderer *, Scene *);
void renderer_compute_shadow(SceneRenderer *, Scene *);

void renderer_close(const SceneRenderer *);
void renderer_draw(SceneRenderer *, Scene *, const SceneRendererDrawMode);

WGPUDevice *renderer_device(SceneRenderer *);
WGPUQueue *renderer_queue(SceneRenderer *);
int renderer_width(SceneRenderer *);
int renderer_height(SceneRenderer *);

const char *renderer_target(SceneRenderer *);


#endif
