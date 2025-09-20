#include "core.h"
#include "runtime/input/input.h"
#include "runtime/scene/core.h"
#include "stdio.h"

SceneEditorUIStatus scene_editor_ui_init(SceneEditorUI *ui,
                                         const SceneEditorUIDescriptor *desc) {

  VERBOSE_PROCESS("Intitializing Editor UI");

  ui->height = desc->height;
  ui->width = desc->width;


  return SceneEditorUIStatus_Success;
}

void scene_editor_ui_draw_callback(void *data) {

  Scene *scene = (Scene *)data;

  
  
}
