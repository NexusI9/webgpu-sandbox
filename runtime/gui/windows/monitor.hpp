#ifndef _GUI_WINDOW_MONITOR_H_
#define _GUI_WINDOW_MONITOR_H_

#include "core.hpp"

namespace UI {

class Monitor : public Window {

public:
  Monitor(Gui* gui, const char *label) : Window(gui, label) {}
  void draw() override;
};

} // namespace UI

#endif
