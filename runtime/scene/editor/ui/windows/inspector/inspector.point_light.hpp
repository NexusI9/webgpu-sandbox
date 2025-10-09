#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_POINT_LIGHT_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_POINT_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"
namespace UI {

class InspectorPointLight : public Window {

public:
  InspectorPointLight(Scene *scene, const char *label, PointLight *light)
      : Window(scene, label), light(light) {}

  void draw() override;

private:
  PointLight *light;
  static constexpr InspectorTreeList<PointLight> attributes = {
      .label = "Properties",
      .float_list =
          {
              .length = 5,
              .entries =
                  {
                      {
                          .label = "Intensity",
                          .accessor_callback = point_light_get_intensity,
                          .mutator_callback = point_light_set_intensity,
                      },
                      {
                          .label = "Cutoff",
                          .accessor_callback = point_light_get_cutoff,
                          .mutator_callback = point_light_set_cutoff,
                      },
                      {
                          .label = "Inner Cutoff",
                          .accessor_callback = point_light_get_inner_cutoff,
                          .mutator_callback = point_light_set_inner_cutoff,
                      },
                      {
                          .label = "Near",
                          .accessor_callback = point_light_get_near,
                          .mutator_callback = point_light_set_near,
                      },
                      {
                          .label = "Far",
                          .accessor_callback = point_light_get_far,
                          .mutator_callback = point_light_set_far,
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
                          .accessor_callback = point_light_get_color,
                          .mutator_callback = point_light_set_color,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
