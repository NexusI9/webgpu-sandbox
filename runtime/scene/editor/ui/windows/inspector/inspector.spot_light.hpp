#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_SPOT_LIGHT_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_SPOT_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"

namespace UI {

class InspectorSpotLight : public Window {

public:
  InspectorSpotLight(Scene *scene, const char *label, SpotLight *light)
      : Window(scene, label), light(light) {}

  void draw() override;

private:
  SpotLight *light;

  static constexpr InspectorTreeList<SpotLight> properties_attributes = {
      .label = "Properties",
      .float_list =
          {
              .length = 4,
              .entries =
                  {
                      {
                          .label = "Intensity",
                          .accessor_callback = spot_light_get_intensity,
                          .mutator_callback = spot_light_set_intensity,
                      },
                      {
                          .label = "Cutoff",
                          .accessor_callback = spot_light_get_cutoff,
                          .mutator_callback = spot_light_set_cutoff,
                      },
                      {
                          .label = "Inner Cutoff",
                          .accessor_callback = spot_light_get_inner_cutoff,
                          .mutator_callback = spot_light_set_inner_cutoff,
                      },
                      {
                          .label = "Angle",
                          .accessor_callback = spot_light_get_angle,
                          .mutator_callback = spot_light_set_angle,
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
                          .accessor_callback = spot_light_get_color,
                          .mutator_callback = spot_light_set_color,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
