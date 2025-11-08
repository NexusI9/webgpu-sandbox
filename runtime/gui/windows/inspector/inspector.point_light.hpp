#ifndef _GUI_WINDOW_INSPECTOR_POINT_LIGHT_H_
#define _GUI_WINDOW_INSPECTOR_POINT_LIGHT_H_

#include "runtime/gui/windows/core.hpp"
#include "runtime/gui/windows/inspector/inspector.hpp"
#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor_mesh/light/point.h"

namespace UI {

class InspectorPointLight : public Window {

public:
  InspectorPointLight(Gui *gui, const char *label, SceneEditorMeshList *sem,
                      const RegEntryType type)
      : Window(gui, label), sem(sem), type(type) {}

  void draw() override;

private:
  PointLight *light;
  SceneEditorMeshList *sem;
  const RegEntryType type;

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
                          .accessor_callback =
                              sem_list_point_light_get_position,
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
              .length = 1,
              .entries =
                  {
                      {
                          .label = "Position",
                          .accessor_callback =
                              sem_list_point_light_get_position,
                          .mutator_callback = nullptr,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };

  InspectorTreeList<PointLight> properties_attributes = {
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
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                      {
                          .label = "Cutoff",
                          .accessor_callback = point_light_get_cutoff,
                          .mutator_callback = point_light_set_cutoff,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                      {
                          .label = "Inner Cutoff",
                          .accessor_callback = point_light_get_inner_cutoff,
                          .mutator_callback = point_light_set_inner_cutoff,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                      {
                          .label = "Near",
                          .accessor_callback = point_light_get_near,
                          .mutator_callback = point_light_set_near,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                      {
                          .label = "Far",
                          .accessor_callback = point_light_get_far,
                          .mutator_callback = point_light_set_far,
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
                          .accessor_callback = point_light_get_color,
                          .mutator_callback = point_light_set_color,
                          .extra_callback = properties_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
