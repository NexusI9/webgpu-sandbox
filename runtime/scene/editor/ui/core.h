#ifndef _SCENE_EDITOR_UI_CORE_H_
#define _SCENE_EDITOR_UI_CORE_H_

typedef enum {
  SceneEditorUIStatus_Success,
  SceneEditorUIStatus_UndefError,
} SceneEditorUIStatus;

typedef struct {
  int *width;
  int *height;
} SceneEditorUI;

typedef struct {
  int *width;
  int *height;
} SceneEditorUIDescriptor;

SceneEditorUIStatus scene_editor_ui_init(SceneEditorUI *,
                                         const SceneEditorUIDescriptor *);

void scene_editor_ui_draw_callback(void *);

#endif
