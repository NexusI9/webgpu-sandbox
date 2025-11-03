#ifndef _GUI_WINDOW_INSPECTOR_MESH_H_
#define _GUI_WINDOW_INSPECTOR_MESH_H_

#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/gui/windows/core.hpp"
#include "runtime/gui/windows/inspector/inspector.hpp"

namespace UI {

  
class InspectorMesh : public Window {

public:
  InspectorMesh(Gui* gui, const char *label, Mesh *mesh)
      : Window(gui, label), mesh(mesh) {}

  void draw() override;

private:
  Mesh *mesh;

  static void transform_update_callback(Scene *, void *);

  InspectorTreeList<Mesh> transform_attributes = {
      .label = "Transform",
      .vec3_list =
          {
              .length = 3,
              .entries =
                  {
                      {
                          .label = "Position",
                          .accessor_callback = mesh_get_position,
                          .mutator_callback = mesh_set_position,
                          .extra_callback = transform_update_callback,
                          .user_data = (void *)mesh,
                      },
                      {
                          .label = "Scale",
                          .accessor_callback = mesh_get_scale,
                          .mutator_callback = mesh_set_scale,
                      },
                      {
                          .label = "Rotation",
                          .accessor_callback = mesh_get_rotation_euler,
                          .mutator_callback = mesh_set_rotation,
                      },
                  },
          },
  };
};

} // namespace UI

#endif
