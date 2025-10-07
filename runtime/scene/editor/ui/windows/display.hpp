#ifndef _SCENE_EDTIOR_UI_WINDOW_DISPLAY_H_
#define _SCENE_EDTIOR_UI_WINDOW_DISPLAY_H_

#include "core.hpp"

namespace UI {

typedef enum {
  UIDisplay_Layout = 1 << 0,
  UIDisplay_Activity = 1 << 1,
} UIDisplay;

class Display : public Window {

public:
  Display(Scene *scene, const char *label, int *state)
      : Window(scene, label), state(state) {}
  int *state;
  void draw() override;
};

} // namespace UI

#endif
