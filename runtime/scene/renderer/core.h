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
  SceneRendererDrawMode_Texture = 1 << 0,
  SceneRendererDrawMode_Solid = 1 << 1,
  SceneRendererDrawMode_Wireframe = 1 << 2,
  SceneRendererDrawMode_Boundbox = 1 << 3,
} SceneRendererDrawMode;

typedef struct {
  cclock *clock;
  WGPUColor background;
  const double dpi;
  const int width;
  const int height;
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

  cclock clock; // update clock delta on draw
  SSBOManager ssbo;
  UBOManager ubo;

  struct {
    double dpi;
    WGPUColor background;
    int width, height;
  } context;

  // cached texture shared throughout parent scene objects
  struct {
    SceneRendererTextureAO ambient_occlusion;
  } texture; // TODO Make a TextureManager

  struct {
    SceneRendererDrawMode mode;
    SceneRendererDrawCallbackList callbacks[SCENE_RENDERER_DRAW_MODE_COUNT];
    RenderPassList render_pass[SCENE_RENDERER_DRAW_MODE_COUNT];
    ComputePass compute_pass;
  } draw;

} SceneRenderer;

typedef struct {
  SceneRenderer *renderer;
} SceneRendererRenderDescriptor;

#ifdef __cplusplus
extern "C" {
#endif

void scene_renderer_init(SceneRenderer *,
                         const SceneRendererCreateDescriptor *);

void scene_renderer_set_draw_mode(SceneRenderer *, const SceneRendererDrawMode);

void scene_renderer_draw_layout_callback(void *);

void scene_renderer_add_draw_callback(SceneRenderer *,
                                      scene_renderer_draw_callback, void *,
                                      const SceneRendererDrawMode);

scene_renderer_draw_callback
scene_renderer_find_draw_callback(SceneRenderer *,
                                  scene_renderer_draw_callback);

void scene_renderer_draw(SceneRenderer *);

// getters
static inline SSBOManager *scene_renderer_ssbo(SceneRenderer *renderer) {
  return &renderer->ssbo;
}
static inline UBOManager *scene_renderer_ubo(SceneRenderer *renderer) {
  return &renderer->ubo;
}

static inline const SceneRendererDrawMode
scene_renderer_draw_mode(SceneRenderer *renderer) {
  return renderer->draw.mode;
}

static inline cclock *scene_renderer_clock(SceneRenderer *renderer) {
  return &renderer->clock;
}

static inline RenderPassList *
scene_renderer_active_pass_list(SceneRenderer *renderer) {
  return &renderer->draw.render_pass[__builtin_ctz(renderer->draw.mode)];
}

static inline int scene_renderer_width(SceneRenderer *rd) {
  return rd->context.width;
}

static inline int scene_renderer_height(SceneRenderer *rd) {
  return rd->context.height;
}

static inline double scene_renderer_dpi(SceneRenderer *rd) {
  return rd->context.dpi;
}

static inline void scene_renderer_set_width(SceneRenderer *rd, int value) {
  rd->context.width = value;
}

static inline void scene_renderer_set_height(SceneRenderer *rd, int value) {
  rd->context.height = value;
}

static inline RenderPassList *
scene_renderer_pass_list(SceneRenderer *renderer,
                         const SceneRendererDrawMode mode) {
  return &renderer->draw.render_pass[__builtin_ctz(mode)];
}

#ifdef __cplusplus
}
#endif

#endif
