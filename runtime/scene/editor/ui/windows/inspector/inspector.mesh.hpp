#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_MESH_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_MESH_H_

#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"

namespace UI {

class InspectorMesh : public Window {

public:
  InspectorMesh(Scene *scene, const char *label, Mesh *mesh)
      : Window(scene, label), mesh(mesh) {}

  void draw() override;

private:
  Mesh *mesh;
  static constexpr InspectorTreeList<Mesh> attributes = {
      .label = "Properties",
      .vec3_list =
          {
              .length = 3,
              .entries =
                  {
                      {
                          .label = "Position",
                          .accessor_callback = mesh_get_position,
                          .mutator_callback = mesh_set_position,
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
