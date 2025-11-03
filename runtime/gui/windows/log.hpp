#ifndef _GUI_WINDOW_LOG_H_
#define _GUI_WINDOW_LOG_H_

#include "core.hpp"

namespace UI {

class Log : public Window {

public:
  Log(Gui* gui, const char *label) : Window(gui, label) {}
  void draw() override;
};

} // namespace UI

#endif
