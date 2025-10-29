#ifndef _SCENE_EDITOR_UI_WINDOW_VERTEX_VIEWER_H_
#define _SCENE_EDITOR_UI_WINDOW_VERTEX_VIEWER_H_

#include "runtime/scene/editor/ui/windows/core.hpp"
namespace UI {

class VertexViewer : public Window {

public:
  VertexViewer(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;
  static bool open;
};

} // namespace UI

#endif
