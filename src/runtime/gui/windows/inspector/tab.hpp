#ifndef _GUI_WINDOW_INSPECTOR_TAB_H_
#define _GUI_WINDOW_INSPECTOR_TAB_H_

#include "runtime/gui/windows/core.hpp"

namespace UI {

class InspectorTab : public Window {

public:
  InspectorTab(Gui *gui, const ThemeIcon icon, const char *label)
      : Window(gui, label), icon(icon), tooltip(label) {}
  virtual void draw() = 0;
  const ThemeIcon icon;
  const char *tooltip;
};

} // namespace UI

#endif
