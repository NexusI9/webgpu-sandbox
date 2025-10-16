#ifndef _SCENE_EDITOR_UI_COMPONENT_SPACING_H_
#define _SCENE_EDITOR_UI_COMPONENT_SPACING_H_

#include "imgui/imgui.h"
#include "runtime/scene/editor/ui/core.h"
namespace UI {

class Spacing {

public:
  Spacing(SceneEditorUI *ui, const SceneEditorUISize size)
      : ui(ui), size(size) {}

  void draw_x() {
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ui->size[size]);
  }

  void draw_y() {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ui->size[size]);
  }

private:
  const SceneEditorUI *ui;
  const SceneEditorUISize size;
};

} // namespace UI

#endif
