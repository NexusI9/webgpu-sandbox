#ifndef _SCENE_EDTIOR_UI_WINDOW_MONITOR_H_
#define _SCENE_EDTIOR_UI_WINDOW_MONITOR_H_

#include "core.hpp"

namespace UI {

class Monitor : public Window {

public:
  Monitor(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;
};

} // namespace UI

#endif
