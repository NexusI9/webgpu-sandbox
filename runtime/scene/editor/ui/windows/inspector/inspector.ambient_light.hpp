#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_AMBIENT_LIGHT_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_AMBIENT_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"

namespace UI {

class InspectorAmbientLight : public Window {

public:
  InspectorAmbientLight(Scene *scene, const char *label, AmbientLight *light)
      : Window(scene, label), light(light) {}

  void draw() override;

private:
  AmbientLight *light;

  static constexpr InspectorTreeList<AmbientLight> attributes = {
      .label = "Properties",
      .float_list =
          {
              .length = 1,
              .entries =
                  {
                      {
                          .label = "Intensity",
                          .accessor_callback = ambient_light_get_intensity,
                          .mutator_callback = ambient_light_set_intensity,
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
                          .accessor_callback = ambient_light_get_color,
                          .mutator_callback = ambient_light_set_color,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
