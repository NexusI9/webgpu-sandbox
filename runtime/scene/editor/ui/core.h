#ifndef _SCENE_EDITOR_UI_CORE_H_
#define _SCENE_EDITOR_UI_CORE_H_

#include "backend/clock.h"
#include "runtime/texture/atlas.h"
#include <webgpu/webgpu.h>

typedef enum {
  SceneEditorUIStatus_Success,
  SceneEditorUIStatus_UndefError,
} SceneEditorUIStatus;

#define SCENE_EDITOR_UI_ICON_COUNT 18

typedef enum {
  SceneEditorUIIcon_Null,
  SceneEditorUIIcon_RenderMode_Boundbox,
  SceneEditorUIIcon_RenderMode_Wireframe,
  SceneEditorUIIcon_RenderMode_Solid,
  SceneEditorUIIcon_RenderMode_Texture,
  SceneEditorUIIcon_Gizmo_Position,
  SceneEditorUIIcon_Gizmo_Rotate,
  SceneEditorUIIcon_Gizmo_Scale,
  SceneEditorUIIcon_Log_Error,
  SceneEditorUIIcon_Log_Warning,
  SceneEditorUIIcon_Log_Info,
  SceneEditorUIIcon_Log_Success,
  SceneEditorUIIcon_Log_Import,
  SceneEditorUIIcon_Log_Process,
  SceneEditorUIIcon_Layout,
  SceneEditorUIIcon_Activity,
  SceneEditorUIIcon_Eye,
  SceneEditorUIIcon_EyeOff,
} SceneEditorUIIcon;

typedef struct {
  ivec2 cell;
  vec2 uv0;
  vec2 uv1;
} SceneEditorUIIconUV;

typedef struct {
  const double dpi;
} SceneEditorUIConfig;

typedef struct {
  cclock *clock;
  double dpi;
  WGPURenderPassEncoder pass_encoder;
  WGPUQuerySet query;

  WGPUTexture depth_texture;
  WGPUTextureView depth_view;
  TextureAtlas atlas_texture;
  SceneEditorUIIconUV icon_uv[SCENE_EDITOR_UI_ICON_COUNT];
} SceneEditorUI;

typedef struct {
  cclock *clock;
  double dpi;
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
