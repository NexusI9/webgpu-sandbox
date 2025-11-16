#ifndef _GUI_WINDOW_BROWSER_H_
#define _GUI_WINDOW_BROWSER_H_

#include "core.hpp"

namespace UI {

class Browser : public Window {

public:
  Browser(Gui* gui, const char *label) : Window(gui, label) {}
  void draw() override;
};

} // namespace UI

#endif
