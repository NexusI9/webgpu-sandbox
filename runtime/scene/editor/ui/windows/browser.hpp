#ifndef _SCENE_EDTIOR_UI_WINDOW_BROWSER_H_
#define _SCENE_EDTIOR_UI_WINDOW_BROWSER_H_

#include "core.hpp"

namespace UI {

class Browser : public Window {

public:
  Browser(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;
};

} // namespace UI

#endif
