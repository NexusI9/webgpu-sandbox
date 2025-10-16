#ifndef _SCENE_EDITOR_UI_COMPONENTS_BUTTON_H_
#define _SCENE_EDITOR_UI_COMPONENTS_BUTTON_H_

#include "imgui/imgui.h"
#include "runtime/scene/editor/ui/core.h"

namespace UI {

class ButtonIcon {

public:
  ButtonIcon(SceneEditorUI *ui, const SceneEditorUIIcon icon, const char *id,
             const ImVec2 size)
      : ui(ui), icon(icon), id(id), size(size) {}
  bool draw();

private:
  SceneEditorUI *ui;
  const SceneEditorUIIcon icon;
  const char *id;
  const ImVec2 size;
};

} // namespace UI

#endif
