#ifndef _SCENE_EDITOR_UI_CORE_H_
#define _SCENE_EDITOR_UI_CORE_H_

#include "backend/clock.h"
#include <webgpu/webgpu.h>

typedef enum {
  SceneEditorUIStatus_Success,
  SceneEditorUIStatus_UndefError,
} SceneEditorUIStatus;

typedef struct {
  int *width;
  int *height;
  cclock *clock;
  WGPURenderPassEncoder pass_encoder;
  WGPUSwapChain *swapchain;
  WGPUDevice device;
  WGPUQueue queue;
  WGPUTexture depth_texture;
  WGPUTextureView depth_view;
} SceneEditorUI;

typedef struct {
  int *width;
  int *height;
  cclock *clock;
  WGPUSwapChain *swapchain;
  WGPUDevice device;
  WGPUQueue queue;
} SceneEditorUIDescriptor;

// prevent c++ mangling
#ifdef __cplusplus
extern "C" {
#endif

SceneEditorUIStatus scene_editor_ui_init(SceneEditorUI *,
                                         const SceneEditorUIDescriptor *);

void scene_editor_ui_draw_callback(void *);

#ifdef __cplusplus
}
#endif

#endif
