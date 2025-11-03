#ifndef _GUI_COMPONENT_SPACING_H_
#define _GUI_COMPONENT_SPACING_H_

#include "imgui/imgui.h"
#include "runtime/gui/core.h"
namespace UI {

class Spacing {

public:
  Spacing(Gui *gui, const ThemeSize size) : gui(gui), size(size) {}

  void draw_x() {
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + theme_size(gui->theme,size));
  }

  void draw_y() {
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + theme_size(gui->theme,size));
  }

private:
  const Gui *gui;
  const ThemeSize size;
};

} // namespace UI

#endif
