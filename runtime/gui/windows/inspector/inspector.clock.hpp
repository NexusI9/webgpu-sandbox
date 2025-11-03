#ifndef _GUI_WINDOW_INSPECTOR_CLOCK_H_
#define _GUI_WINDOW_INSPECTOR_CLOCK_H_

#include "backend/profiler.h"
#include "backend/theme/core.h"
#include "runtime/gui/windows/inspector/tab.hpp"

namespace UI {

typedef struct {
  const ProfilerLatencyType type;
  const char *label;
  const float *color;
  double value;
} ClockTabBar;

class ClockTab : public InspectorTab {

public:
  ClockTab(Gui *gui, const ThemeIcon icon, const char *label)
      : InspectorTab(gui, icon, label) {}
  void draw() override;

private:
  static constexpr double max_value = 10.0f;
  static constexpr color color = {0.3f, 0.3f, 0.5f, 1.0f};
};

} // namespace UI

#endif
