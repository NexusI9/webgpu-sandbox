#ifndef _GUI_WINDOW_DISPLAY_H_
#define _GUI_WINDOW_DISPLAY_H_

#include "core.hpp"
#include "runtime/gui/core.h"

namespace UI {

typedef enum {
  DisplayState_Layout = 1 << 0,
  DisplayState_Activity = 1 << 1,
  DisplayState_Grid = 1 << 2,
  DisplayState_Probe = 1 << 3,
  DisplayState_Light = 1 << 4,
} DisplayState;

class Display : public Window {

public:
  Display(Gui* gui, const char *label) : Window(gui, label) {}
  static int state;
  void draw() override;

private:
  static constexpr int filter_length = 5;

  static void checkbox_on_change_base(Scene *, bool, void *);
  static void checkbox_on_change_grid(Scene *, bool, void *);
  static void checkbox_on_change_probe(Scene *, bool, void *);
  static void checkbox_on_change_light(Scene *, bool, void *);
  static inline void checkbox_update_state(bool, const DisplayState);

  static constexpr struct {
    const DisplayState target_state;
    const ThemeIcon icon;
    const char *label;
    void (*on_change)(Scene *, bool, void *);
  } filters[filter_length] = {
      {
          DisplayState_Layout,
          ThemeIcon_Layout,
          "Layout",
          checkbox_on_change_base,
      },
      {
          DisplayState_Activity,
          ThemeIcon_Activity,
          "Activity",
          checkbox_on_change_base,
      },
      {
          DisplayState_Grid,
          ThemeIcon_Grid,
          "Grid",
          checkbox_on_change_grid,
      },
      {
          DisplayState_Probe,
          ThemeIcon_ProbeReflectionGrid,
          "Probes",
          checkbox_on_change_probe,
      },
      {
          DisplayState_Light,
          ThemeIcon_PointLight,
          "Lights",
          checkbox_on_change_light,
      },
  };
};

} // namespace UI

#endif
