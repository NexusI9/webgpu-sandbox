#ifndef _GUI_WINDOW_GIZMO_H_
#define _GUI_WINDOW_GIZMO_H_

#include "core.hpp"

namespace UI {

class Gizmo : public Window {

public:
  Gizmo(Gui* gui, const char *label) : Window(gui, label) {}
  void draw() override;
};

} // namespace UI

#endif
