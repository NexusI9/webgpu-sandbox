#ifndef _SCENE_EDTIOR_UI_WINDOW_HIERARCHY_H_
#define _SCENE_EDTIOR_UI_WINDOW_HIERARCHY_H_

#include "core.hpp"

namespace UI {

class Hierarchy : public Window {

public:
  Hierarchy(Scene *scene, const char *label) : Window(scene, label){}
  void draw() override;
};

} // namespace UI

#endif
