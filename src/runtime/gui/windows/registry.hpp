#ifndef _GUI_WINDOW_REGISTRY_H_
#define _GUI_WINDOW_REGISTRY_H_

#include "runtime/gui/windows/core.hpp"
namespace UI {

class Registry : public Window {

public:
  Registry(Gui *gui, const char *label) : Window(gui, label) {}
  void draw() override;
  static bool open;
};

} // namespace UI

#endif
