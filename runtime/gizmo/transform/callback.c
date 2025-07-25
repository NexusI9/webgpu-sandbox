#include "callback.h"

void gizmo_transform_callback_translate(GizmoTransform *gizmo,
                                        MeshRefList *list, Camera *camera,
                                        Viewport *viewport) {

  const Axis axis = gizmo->axis;
  float x = g_input.mouse.movement.x;
  float y = g_input.mouse.movement.y;

  vec3 *camera_axis[] = {
      [Axis_X] = &camera->right,
      [Axis_Y] = &camera->up,
      [Axis_Z] = &camera->forward,
  };

  vec2 mouse_vec = {x, y};

  // project 3D axis to screen to get direction
  vec3 origin, axis_point;
  // copy first gizmo hangle position as origin
  glm_vec3_copy(gizmo->handles[gizmo->mode].entries[0]->position, origin);
  // slightly move the origin point along the target axis
  glm_vec3_add(origin, *camera_axis[axis], axis_point);

  // project origin and point to screen space
  // (works the same as Model/View matrix system, just need to multiply the vec3
  // with the projection matrix to change its space to screen space)
  mat4 view_proj;
  glm_mat4_mul(viewport->projection, camera->view, view_proj);

  vec4 screen_origin, screen_point;
  glm_mat4_mulv(view_proj, origin, screen_origin);
  glm_mat4_mulv(view_proj, axis_point, screen_point);

  // once projected, get the NDC by dividing with with Z value
  glm_vec4_divs(screen_origin, screen_origin[3], screen_origin);
  glm_vec4_divs(screen_point, screen_point[3], screen_point);

  // get delta of two projected point
  vec2 axis_dir = {
      screen_point[0] - screen_origin[0],
      screen_point[1] - screen_origin[1],
  };

  glm_vec2_normalize(axis_dir);

  float sign = glm_vec2_dot(mouse_vec, axis_dir);
  // printf("sign: %f\n", sign);
  float sensi = 0.1; // sensi

  float delta = glm_vec2_norm(mouse_vec) * glm_signf(sign) * sensi;

  //printf("x:%f,y:%f\t∆:%f\tsign:%f\n", x, y, delta, sign);

  vec3 movement;
  glm_vec3_scale(*camera_axis[axis], delta, movement);

  // move meshes
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_axis_add(list->entries[i], movement[axis], axis);

  // move gizmo
  gizmo_transform_translate_add(gizmo, movement[axis], axis);
}

void gizmo_transform_callback_rotate(GizmoTransform *gizmo, MeshRefList *list,
                                     Camera *camera, Viewport *viewport) {}

void gizmo_transform_callback_scale(GizmoTransform *gizmo, MeshRefList *list,
                                    Camera *camera, Viewport *viewport) {}
