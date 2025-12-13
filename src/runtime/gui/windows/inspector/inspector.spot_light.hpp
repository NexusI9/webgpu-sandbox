#ifndef _GUI_WINDOW_INSPECTOR_SPOT_LIGHT_H_
#define _GUI_WINDOW_INSPECTOR_SPOT_LIGHT_H_

#include "runtime/gui/windows/core.hpp"
#include "runtime/gui/windows/inspector/inspector.hpp"
#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/editor_mesh/light/spot.h"

namespace UI {

class InspectorSpotLight : public Window {

public:
  InspectorSpotLight(Gui *gui, const char *label, SceneEditorMeshList *sem,
                     const RegEntryType type)
      : Window(gui, label), sem(sem), type(type) {}

  void draw() override;

private:
  SpotLight *light;
  SceneEditorMeshList *sem;
  const RegEntryType type;

  static void transform_update_callback(Scene *, Renderer *, void *);
  static void properties_update_callback(Scene *, Renderer *, void *);

  InspectorTreeList<SceneEditorMeshList> transform_attributes = {
      .label = "Transform",
      .vec3_list =
          {
              .count = 1,
              .entries =
                  {
                      {
                          .label = "Position",
                          .accessor_callback = sem_list_spot_light_get_position,
                          .mutator_callback = nullptr,
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
              .count = 1,
              .entries =
                  {
                      {
                          .label = "Position",
                          .accessor_callback = sem_list_spot_light_get_position,
                          .mutator_callback = nullptr,
                          .extra_callback = transform_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };

  InspectorTreeList<SpotLight> properties_attributes = {
      .label = "Properties",
      .float_list =
          {
              .count = 4,
              .entries =
                  {
                      {
                          .label = "Intensity",
                          .accessor_callback = spot_light_get_intensity,
                          .mutator_callback = spot_light_set_intensity,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                      {
                          .label = "Cutoff",
                          .accessor_callback = spot_light_get_cutoff,
                          .mutator_callback = spot_light_set_cutoff,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                      {
                          .label = "Inner Cutoff",
                          .accessor_callback = spot_light_get_inner_cutoff,
                          .mutator_callback = spot_light_set_inner_cutoff,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                      {
                          .label = "Angle",
                          .accessor_callback = spot_light_get_angle,
                          .mutator_callback = spot_light_set_angle,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
      .color_list =
          {
              .count = 1,
              .entries =
                  {
                      {
                          .label = "Color",
                          .accessor_callback = spot_light_get_color,
                          .mutator_callback = spot_light_set_color,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
