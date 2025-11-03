#ifndef _GUI_WINDOW_INSPECTOR_AMBIENT_LIGHT_H_
#define _GUI_WINDOW_INSPECTOR_AMBIENT_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/mesh/light/ambient.h"
#include "runtime/gui/windows/core.hpp"
#include "runtime/gui/windows/inspector/inspector.hpp"

namespace UI {

class InspectorAmbientLight : public Window {

public:
  InspectorAmbientLight(Gui* gui, const char *label,
                        SceneEditorMeshList *sem)
      : Window(gui, label), sem(sem) {}

  void draw() override;

private:
  SceneEditorMeshList *sem;
  AmbientLight *light;

  static void transform_update_callback(Scene *, void *);
  static void properties_update_callback(Scene *, void *);

  InspectorTreeList<SceneEditorMeshList> transform_attributes = {
      .label = "Transform",
      .vec3_list =
          {
              .length = 1,
              .entries =
                  {
                      {
                          .label = "Position",
                          .accessor_callback =
                              sem_list_ambient_light_get_position,
                          .mutator_callback =
                              sem_list_ambient_light_set_position,
                          .extra_callback = transform_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };

  InspectorTreeList<AmbientLight> properties_attributes = {
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
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
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
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
