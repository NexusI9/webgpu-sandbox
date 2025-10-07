#ifndef _SCENE_EDTIOR_UI_WINDOW_GIZMO_H_
#define _SCENE_EDTIOR_UI_WINDOW_GIZMO_H_

#include "core.hpp"

namespace UI {

class Gizmo : public Window {

public:
  Gizmo(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;
};

} // namespace UI

#endif
