#include "callback.h"

void gizmo_transform_callback_translate(GizmoTransform *gizmo,
                                        MeshRefList *list, Camera *camera) {

  const Axis axis = gizmo->axis;
  float x = g_input.mouse.movement.x;
  float y = g_input.mouse.movement.y;

  // printf("x: %d, y:%d\n", g_input.mouse.delta.x, g_input.mouse.delta.y);
  printf("x: %f, y:%f\n", x, y);

  vec3 *camera_axis[] = {
      [Axis_X] = &camera->right,
      [Axis_Y] = &camera->up,
      [Axis_Z] = &camera->forward,
  };

  vec3 axis_dir;
  glm_vec3_copy(*camera_axis[gizmo->axis], axis_dir);

  vec2 mouse_vec = {x, -y};
  float scale = 0.01; // sensi
  float delta = glm_vec2_norm(mouse_vec) * scale;

  vec3 movement;
  glm_vec3_scale(axis_dir, delta, movement);

  // move meshes
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_axis_add(list->entries[i], movement[axis], axis);

  // move gizmo
  gizmo_transform_translate_add(gizmo, movement[axis], axis);
}

void gizmo_transform_callback_rotate(GizmoTransform *gizmo, MeshRefList *list,
                                     Camera *camera) {}

void gizmo_transform_callback_scale(GizmoTransform *gizmo, MeshRefList *list,
                                    Camera *camera) {}
