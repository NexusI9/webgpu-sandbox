#ifndef _SCENE_EDITOR_UI_COMPONENT_TIME_BAR_H_
#define _SCENE_EDITOR_UI_COMPONENT_TIME_BAR_H_

#include "imgui/imgui.h"
#include "runtime/scene/editor/ui/components/core.hpp"

namespace UI {

typedef struct {
  const ImVec4 background, bar;
  const float border_radius;
  const float height;
} TimeBarStyle;

class TimeBar : public Component {

public:
  TimeBar(Scene *scene, const char *label, const double value,
          const double max_value, const TimeBarStyle *style)
      : Component(scene, label), value(value), max_value(max_value), style(style) {
  }

  bool draw() override;

private:
  const double value;
  const double max_value;
  const TimeBarStyle *style;
};

} // namespace UI

#endif
