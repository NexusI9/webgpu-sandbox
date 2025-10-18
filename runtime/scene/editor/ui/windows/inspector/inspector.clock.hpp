#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_CLOCK_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_CLOCK_H_

#include "backend/profiler.h"
#include "runtime/scene/editor/ui/windows/inspector/tab.hpp"

namespace UI{


  typedef struct {
  const ProfilerLatencyType type;
  const char *label;
  const float *color;
  double value;
} ClockTabBar;

class ClockTab : public InspectorTab {

public:
  ClockTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;

private:
  static constexpr double max_value = 10.0f;
  static constexpr color color = {0.3f, 0.3f, 0.5f, 1.0f};
};

  
  
}


#endif
