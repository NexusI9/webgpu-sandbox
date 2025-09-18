#ifndef _SCENE_EDITOR_UI_CORE_H_
#define _SCENE_EDITOR_UI_CORE_H_

typedef enum {
  SceneUIStatus_Success,
  SceneUIStatus_UndefError,
} SceneEditorUIStatus;

typedef struct {

} SceneEditorUI;

SceneEditorUIStatus scene_ui_init(SceneEditorUI *);

void scene_ui_draw_callback(void *);

#endif
