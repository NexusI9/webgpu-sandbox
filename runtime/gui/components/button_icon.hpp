#ifndef _GUI_COMPONENTS_BUTTON_H_
#define _GUI_COMPONENTS_BUTTON_H_

#include "imgui/imgui.h"
#include "runtime/gui/core.h"

namespace UI {

class ButtonIcon {

public:
  ButtonIcon(Gui *gui, const ThemeIcon icon, const char *id, const ImVec2 size)
      : gui(gui), icon(icon), id(id), size(size) {}
  bool draw();

private:
  Gui *gui;
  const ThemeIcon icon;
  const char *id;
  const ImVec2 size;
};

} // namespace UI

#endif
