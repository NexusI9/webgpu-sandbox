#ifndef _GUI_WINDOW_CORE_H_
#define _GUI_WINDOW_CORE_H_

#include "runtime/gui/core.h"
#include "runtime/scene/core.h"

namespace UI {

class Window {

public:
  Window(Gui *gui, const char *label)
      : scene(gui->active_scene), renderer(gui->renderer), gui(gui),
        label(label) {}
  virtual void draw() = 0;

protected:
  const char *label;
  Scene *scene;
  Renderer *renderer;
  Gui *gui;
};

} // namespace UI

#endif
