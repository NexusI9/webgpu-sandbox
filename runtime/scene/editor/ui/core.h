#ifndef _SCENE_EDITOR_UI_CORE_H_
#define _SCENE_EDITOR_UI_CORE_H_

#include "backend/clock.h"
#include "backend/registry.h"
#include "emscripten/html5.h"
#include "runtime/scene/editor/ui/tree.h"
#include "runtime/texture/atlas.h"
#include <webgpu/webgpu.h>

typedef enum {
  SceneEditorUIStatus_Success,
  SceneEditorUIStatus_UndefError,
} SceneEditorUIStatus;

#define SCENE_EDITOR_UI_ICON_COUNT 32
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
  SceneEditorUIIcon_Properties_Scene,
  SceneEditorUIIcon_Properties_Setting,
  SceneEditorUIIcon_Properties_Object,
  SceneEditorUIIcon_Properties_Chip,
  SceneEditorUIIcon_Properties_Clock,
  SceneEditorUIIcon_PointLight,
  SceneEditorUIIcon_AmbientLight,
  SceneEditorUIIcon_SunLight,
  SceneEditorUIIcon_SpotLight,
  SceneEditorUIIcon_Mesh,
  SceneEditorUIIcon_ProbeReflectionPlane,
  SceneEditorUIIcon_ProbeReflectionGrid,
  SceneEditorUIIcon_Camera,
  SceneEditorUIIcon_Grid,
} SceneEditorUIIcon;

#define SCENE_EDITOR_UI_SIZE_COUNT 23
typedef enum {
  SceneEditorUISize_Screen_Width,
  SceneEditorUISize_Screen_Height,
  SceneEditorUISize_RightPanel_Width,
  SceneEditorUISize_RightPanelTab_Width,
  SceneEditorUISize_Tree_Height,
  SceneEditorUISize_Tree_PaddingV,
  SceneEditorUISize_Tree_PaddingH,
  SceneEditorUISize_TopBar_Height,
  SceneEditorUISize_TopBar_Margin,
  SceneEditorUISize_Gizmo_Width,
  SceneEditorUISize_Gizmo_Height,
  SceneEditorUISize_Gizmo_Margin,
  SceneEditorUISize_Button_RenderModeSize,
  SceneEditorUISize_Button_GizmoSize,
  SceneEditorUISize_Button_InspectorTab,
  SceneEditorUISize_Button_DisplaySize,
  SceneEditorUISize_BottomPanel_Height,
  SceneEditorUISize_Log_IconScale,
  SceneEditorUISize_Monitor_Width,
  SceneEditorUISize_Monitor_Height,
  SceneEditorUISize_Space_Small,
  SceneEditorUISize_Space_Medium,
  SceneEditorUISize_Space_Large,
} SceneEditorUISize;

typedef struct {
  ivec2 cell;
  vec2 uv0;
  vec2 uv1;
} SceneEditorUIIconUV;

typedef struct {
  const double dpi;
} SceneEditorUIConfig;

typedef struct {

  reg_id_t id;
  cclock *clock;
  double dpi;
  WGPURenderPassEncoder pass_encoder;
  WGPUQuerySet query;

  WGPUTexture depth_texture;
  WGPUTextureView depth_view;
  TextureAtlas atlas_texture;

  SceneEditorUITree tree;
  SceneEditorUIIconUV icon_uv[SCENE_EDITOR_UI_ICON_COUNT];
  int size[SCENE_EDITOR_UI_SIZE_COUNT];

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

bool scene_editor_keydown_callback(int eventType,
                                   const EmscriptenKeyboardEvent *keyEvent,
                                   void *userData);

static inline int scene_editor_ui_size(const SceneEditorUI *ui, const int size) {
  return size * ui->dpi;
}

#ifdef __cplusplus
}
#endif

#endif
