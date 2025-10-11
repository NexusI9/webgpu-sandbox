#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_SUN_LIGHT_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_SUN_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"

namespace UI {

class InspectorSunLight : public Window {

public:
  InspectorSunLight(Scene *scene, const char *label, SunLight *light)
      : Window(scene, label), light(light) {}

  void draw() override;

private:
  SunLight *light;
  static constexpr InspectorTreeList<SunLight> properties_attributes = {
      .label = "Properties",
      .float_list =
          {
              .length = 1,
              .entries =
                  {
                      {
                          .label = "Intensity",
                          .accessor_callback = sun_light_get_intensity,
                          .mutator_callback = sun_light_set_intensity,
                      },
                  },
          },
      .color_list =
          {
              .length = 1,
              .entries =
                  {
                      {
                          .label = "Color",
                          .accessor_callback = sun_light_get_color,
                          .mutator_callback = sun_light_set_color,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
