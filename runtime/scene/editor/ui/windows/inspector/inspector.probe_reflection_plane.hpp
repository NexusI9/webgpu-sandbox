#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_PROBE_REFLECTION_PLANE_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_PROBE_REFLECTION_PLANE_H_

#include "runtime/light/core.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/mesh/light/ambient.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"

namespace UI {

class InspectorProbeReflectionPlane : public Window {

public:
  InspectorProbeReflectionPlane(Scene *scene, const char *label,
                                SceneEditorMeshList *sem)
      : Window(scene, label), sem(sem) {}

  void draw() override;

private:
  SceneEditorMeshList *sem;
  ProbeReflectionPlane *probe;

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
                          .extra_callback = transform_update_callback,
                          .user_data = (void *)sem,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
