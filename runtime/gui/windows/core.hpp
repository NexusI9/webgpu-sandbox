#ifndef _GUI_WINDOW_CORE_H_
#define _GUI_WINDOW_CORE_H_

#include "runtime/scene/core.h"
#include "runtime/gui/core.h"

namespace UI {

class Window {

public:
  Window(Gui *gui, const char *label)
      : scene(gui->active_scene), gui(gui), label(label) {}
  virtual void draw() = 0;

protected:
  const char *label;
  Scene *scene;
  Gui *gui;
};

} // namespace UI

#endif
