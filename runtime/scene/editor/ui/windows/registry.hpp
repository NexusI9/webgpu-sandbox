#ifndef _SCENE_EDITOR_UI_WINDOW_REGISTRY_H_
#define _SCENE_EDITOR_UI_WINDOW_REGISTRY_H_

#include "runtime/scene/editor/ui/windows/core.hpp"
namespace UI {

class Registry : public Window {

public:
  Registry(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;
  static bool open;
};

} // namespace UI

#endif
