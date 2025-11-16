#ifndef _GUI_WINDOW_INSPECTOR_SUN_LIGHT_H_
#define _GUI_WINDOW_INSPECTOR_SUN_LIGHT_H_

#include "backend/registry.h"
#include "runtime/gui/windows/core.hpp"
#include "runtime/gui/windows/inspector/inspector.hpp"
#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor_mesh/list.h"
#include "runtime/scene/editor_mesh/light/sun.h"

namespace UI {

class InspectorSunLight : public Window {

public:
  InspectorSunLight(Gui *gui, const char *label, SceneEditorMeshList *sem,
                    const RegEntryType type)
      : Window(gui, label), sem(sem), type(type) {}

  void draw() override;

private:
  const RegEntryType type;
  SunLight *light;
  SceneEditorMeshList *sem;

  static void transform_update_callback(Scene *, Renderer *, void *);
  static void properties_update_callback(Scene *, Renderer *, void *);

  InspectorTreeList<SceneEditorMeshList> transform_attributes = {
      .label = "Transform",
      .vec3_list =
          {
              .length = 1,
              .entries =
                  {
                      {
                          .label = "Position",
                          .accessor_callback = sem_list_sun_light_get_position,
                          .mutator_callback = NULL,
                          .extra_callback = transform_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };

  InspectorTreeList<SceneEditorMeshList> transform_shadow_attributes = {
      .label = "Transform",
      .vec3_list =
          {
              .length = 1,
              .entries =
                  {
                      {
                          .label = "Position",
                          .accessor_callback = sem_list_sun_light_get_position,
                          .mutator_callback = NULL,
                          .extra_callback = transform_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };

  InspectorTreeList<SunLight> properties_attributes = {
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
                          .accessor_callback = sun_light_get_color,
                          .mutator_callback = sun_light_set_color,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
