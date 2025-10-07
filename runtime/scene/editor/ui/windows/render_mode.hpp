#ifndef _SCENE_EDTIOR_UI_WINDOW_RENDER_MODE_H_
#define _SCENE_EDTIOR_UI_WINDOW_RENDER_MODE_H_

#include "core.hpp"

namespace UI {

class RenderMode : public Window {

public:
  RenderMode(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;
};

} // namespace UI

#endif
