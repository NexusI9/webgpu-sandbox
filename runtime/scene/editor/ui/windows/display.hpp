#ifndef _SCENE_EDTIOR_UI_WINDOW_DISPLAY_H_
#define _SCENE_EDTIOR_UI_WINDOW_DISPLAY_H_

#include "core.hpp"
#include "runtime/scene/editor/ui/core.h"

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
  Display(Scene *scene, const char *label) : Window(scene, label) {}
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
    const SceneEditorUIIcon icon;
    const char *label;
    void (*on_change)(Scene *, bool, void *);
  } filters[filter_length] = {
      {
          DisplayState_Layout,
          SceneEditorUIIcon_Layout,
          "Layout",
          checkbox_on_change_base,
      },
      {
          DisplayState_Activity,
          SceneEditorUIIcon_Activity,
          "Activity",
          checkbox_on_change_base,
      },
      {
          DisplayState_Grid,
          SceneEditorUIIcon_Grid,
          "Grid",
          checkbox_on_change_grid,
      },
      {
          DisplayState_Probe,
          SceneEditorUIIcon_ProbeReflectionGrid,
          "Probes",
          checkbox_on_change_probe,
      },
      {
          DisplayState_Light,
          SceneEditorUIIcon_PointLight,
          "Lights",
          checkbox_on_change_light,
      },
  };
};

} // namespace UI

#endif
