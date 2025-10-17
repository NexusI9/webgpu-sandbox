#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_SPOT_LIGHT_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_SPOT_LIGHT_H_

#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/mesh/light/spot.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"

namespace UI {

class InspectorSpotLight : public Window {

public:
  InspectorSpotLight(Scene *scene, const char *label, SceneEditorMeshList *sem,
                     const RegEntryType type)
      : Window(scene, label), sem(sem), type(type) {}

  void draw() override;

private:
  SpotLight *light;
  SceneEditorMeshList *sem;
  const RegEntryType type;

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
                          .accessor_callback = sem_list_spot_light_get_position,
                          .mutator_callback = sem_list_spot_light_set_position,
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
                          .accessor_callback = sem_list_spot_light_get_position,
                          .mutator_callback =
                              sem_list_spot_light_shadow_set_position,
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
              .length = 4,
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
              .length = 1,
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
