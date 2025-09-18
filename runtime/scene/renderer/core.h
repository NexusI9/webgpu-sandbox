#ifndef _SCENE_RENDERER_CORE_H_
#define _SCENE_RENDERER_CORE_H_

#include <emscripten/html5.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include "./render_pass/render_pass.h"
#include "backend/ao_bake/ao_bake.h"
#include "backend/ao_bake/core.h"
#include "backend/clock.h"
#include "backend/compute/core.h"
#include "backend/ssbo.h"
#include "backend/ubo.h"
#include "render_pass/core.h"
#include "runtime/pipeline/pipeline.h"
#include "runtime/pipeline/render.h"
#include "runtime/texture/core.h"
#include "runtime/texture/texture.h"
#include "webgpu/webgpu.h"

#define SCENE_RENDERER_MAX_HOOK 6
#define SCENE_RENDERER_DPI_AUTO 0
#define SCENE_RENDERER_DRAW_MODE_COUNT 4

typedef enum {
  SceneRendererDrawMode_Texture,
  SceneRendererDrawMode_Solid,
  SceneRendererDrawMode_Wireframe,
  SceneRendererDrawMode_Boundbox,
} SceneRendererDrawMode;

typedef struct {
  const char *name;
  cclock *clock;
  WGPUColor background;
  const double dpi;
  const  RenderPipelineMultisampleCount multisampling_count;
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

typedef struct SceneRenderer {

  cclock clock;         // update clock delta on draw
  WGPUColor background; // TODO: put this under context
  SSBOManager ssbo;
  UBOManager ubo;

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
    WGPURenderPipeline pipeline;
    WGPUSwapChain swapchain;
    WGPURenderPassEncoder render_pass;
  } wgpu;

  // cached texture shared throughout parent scene objects
  struct {
    SceneRendererTextureAO ambient_occlusion;
  } texture; // TODO Make a TextureManager

  struct {
    SceneRendererDrawMode mode;
    SceneRendererDrawCallbackList callbacks;
    RenderPassList render_pass[SCENE_RENDERER_DRAW_MODE_COUNT];
    ComputePass compute_pass;
  } draw;

} SceneRenderer;

typedef struct {
  SceneRenderer *renderer;
} SceneRendererRenderDescriptor;

void scene_renderer_init(SceneRenderer *,
                         const SceneRendererCreateDescriptor *);

void scene_renderer_set_draw_mode(SceneRenderer *, const SceneRendererDrawMode);

void scene_renderer_draw_layout_callback(void *);

void scene_renderer_add_draw_callback(SceneRenderer *,
                                      scene_renderer_draw_callback, void *);
void scene_renderer_draw(SceneRenderer *);
void scene_renderer_close(const SceneRenderer *);

// getters
WGPUDevice scene_renderer_device(SceneRenderer *);
WGPUQueue scene_renderer_queue(SceneRenderer *);
WGPUSwapChain scene_renderer_swapchain(SceneRenderer *);
int scene_renderer_width(const SceneRenderer *);
int scene_renderer_height(const SceneRenderer *);
SSBOManager *scene_renderer_ssbo(SceneRenderer *);
UBOManager *scene_renderer_ubo(SceneRenderer *);
// PipelineMultisampleCount scene_renderer_multisample(const SceneRenderer *);

const char *scene_renderer_target(SceneRenderer *);

const SceneRendererDrawMode scene_renderer_draw_mode(SceneRenderer *);

bool scene_renderer_resize_callback(int, const EmscriptenUiEvent *, void *);
cclock *scene_renderer_clock(SceneRenderer *);

#endif
