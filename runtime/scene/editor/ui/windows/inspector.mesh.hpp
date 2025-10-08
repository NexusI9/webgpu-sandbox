#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_MESH_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_MESH_H_

#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
namespace UI {

typedef struct {
  const char *label;
  void (*mutator_callback)(Mesh *, vec3);
  void (*accessor_callback)(Mesh *, vec3);
  vec3 attribute;
} InspectorMeshTransformLayout;

class InspectorMesh : public Window {

public:
  InspectorMesh(Scene *scene, const char *label, Mesh *mesh)
      : Window(scene, label), mesh(mesh) {}

  void draw() override;

private:
  Mesh *mesh;
  vec3 position, scale, rotation;

  static constexpr uint8_t transform_layout_len = 3;
  static constexpr InspectorMeshTransformLayout
      transform_layout[transform_layout_len] = {
          {
              .label = "Position",
              .accessor_callback = mesh_get_position,
              .mutator_callback = mesh_set_position,
              .attribute = {0},
          },
          {
              .label = "Scale",
              .accessor_callback = mesh_get_scale,
              .mutator_callback = mesh_set_scale,
              .attribute = {0},
          },
          {
              .label = "Rotation",
              .accessor_callback = mesh_get_rotation_euler,
              .mutator_callback = mesh_set_rotation,
              .attribute = {0},
          },
      };
};

} // namespace UI

#endif
