#ifndef _SCENE_EDTIOR_UI_WINDOW_LOG_H_
#define _SCENE_EDTIOR_UI_WINDOW_LOG_H_

#include "core.hpp"

namespace UI {

class Log : public Window {

public:
  Log(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;
};

} // namespace UI

#endif
