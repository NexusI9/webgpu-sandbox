#ifndef _SCENE_RENDERER_CORE_H_
#define _SCENE_RENDERER_CORE_H_

#include "../../clock.h"
#include "../runtime/pipeline/pipeline.h"
#include "../runtime/texture/texture.h"
#include "render_pass.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

#define SCENE_RENDERER_MAX_HOOK 6
#define SCENE_RENDERER_DPI_AUTO 0
#define SCENE_RENDERER_DRAW_MODE_COUNT 6

typedef enum {
  SceneRendererDrawMode_Texture,
  SceneRendererDrawMode_Solid,
  SceneRendererDrawMode_Wireframe,
  SceneRendererDrawMode_Boundbox,
  SceneRendererDrawMode_Fixed,
  SceneRendererDrawMode_Selection,
} SceneRendererDrawMode;

typedef struct {
  const char *name;
  cclock *clock;
  PipelineMultisampleCount multisampling_count;
  WGPUColor background;
  double dpi;
} SceneRendererCreateDescriptor;

typedef void (*scene_renderer_draw_callback)(void *);

typedef struct {
  scene_renderer_draw_callback callback;
  void *data;
} SceneRendererDrawCallback;

typedef struct {
  SceneRendererDrawCallback entries[SCENE_RENDERER_MAX_HOOK];
  ssize_t length;
} SceneRendererDrawCallbackList;

typedef struct {
  WGPUTextureView depth;
  WGPUTextureView color;
} SceneRendererTextureRender;

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
    WGPURenderPassEncoder render_pass;
  } wgpu;

  struct {
    SceneRendererTextureRender render;
    PipelineMultisampleCount multisample;
  } texture;

  struct {
    SceneRendererDrawMode mode;
    SceneRendererDrawCallbackList callbacks;
    RenderPassLayout layouts[SCENE_RENDERER_DRAW_MODE_COUNT];
    RenderPass pass[RENDER_PASS_COUNT];
  } draw;

} SceneRenderer;

typedef struct {
  SceneRenderer *renderer;
} SceneRendererRenderDescriptor;

void scene_renderer_create(SceneRenderer *,
                           const SceneRendererCreateDescriptor *);

void scene_renderer_set_draw_mode(SceneRenderer *, const SceneRendererDrawMode);

void scene_renderer_set_draw_layout(SceneRenderer *,
                                    const SceneRendererDrawMode,
                                    const RenderPassLayout *);

RenderPass *scene_renderer_pass(SceneRenderer *, const RenderPassType);

void scene_renderer_draw_layout_callback(void *);

void scene_renderer_add_draw_callback(SceneRenderer *,
                                      scene_renderer_draw_callback, void *);
void scene_renderer_draw(SceneRenderer *);
void scene_renderer_close(const SceneRenderer *);

// getters
WGPUDevice scene_renderer_device(SceneRenderer *);
WGPUQueue scene_renderer_queue(SceneRenderer *);
int scene_renderer_width(const SceneRenderer *);
int scene_renderer_height(const SceneRenderer *);

const char *scene_renderer_target(SceneRenderer *);

const SceneRendererDrawMode scene_renderer_draw_mode(SceneRenderer *);

bool scene_renderer_resize_callback(int, const EmscriptenUiEvent *, void *);

#endif
