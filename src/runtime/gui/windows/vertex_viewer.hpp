#ifndef _GUI_WINDOW_VERTEX_VIEWER_H_
#define _GUI_WINDOW_VERTEX_VIEWER_H_

#include "runtime/gui/windows/core.hpp"
namespace UI {

class VertexViewer : public Window {

public:
  VertexViewer(Gui *gui, const char *label) : Window(gui, label) {}
  void draw() override;
  static bool open;
};

} // namespace UI

#endif
