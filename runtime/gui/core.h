#ifndef _GUI_CORE_H_
#define _GUI_CORE_H_

#include "backend/clock.h"
#include "backend/registry.h"
#include "backend/renderer/core.h"
#include "backend/theme/core.h"
#include "emscripten/html5.h"
#include "runtime/gui/tree.h"
#include "runtime/scene/core.h"
#include "runtime/texture/atlas.h"
#include <webgpu/webgpu.h>

#define _GUI_MAX_SCENE_COUNT 1

typedef enum {
  GUIStatus_Success,
  GUIStatus_UndefError,
} GUIStatus;

typedef struct {
  const double dpi;
} GUIConfig;

typedef struct {

  reg_id_t id;
  double dpi;
  WGPURenderPassEncoder pass_encoder;
  WGPUQuerySet query;

  WGPUTexture depth_texture;
  WGPUTextureView depth_view;

  GUITree tree;
  Scene *active_scene;
  Renderer *renderer; // TODO make a gui_system instead of passing scene and
                      // renderer in it
  Theme *theme;

} Gui;

typedef struct {
  double dpi;
  Theme *theme;
  Scene *active_scene;
  Renderer *renderer;
} GUIDescriptor;

// prevent c++ mangling
EXTERN_C_BEGIN

GUIStatus gui_init(Gui *, const GUIDescriptor *);
void gui_destroy(Gui *);

void gui_draw_callback(Renderer *, void *);

bool keydown_callback(int eventType, const EmscriptenKeyboardEvent *keyEvent,
                      void *userData);

static inline int gui_size(const Gui *ui, const int size) {
  return size * ui->dpi;
}

EXTERN_C_END

#endif
