#include "callback.h"
#include "core.h"
#include "utils.h"

void gizmo_transform_callback_movement(GizmoTransform *gizmo, MeshRefList *list,
                                       Camera *camera, Viewport *viewport) {

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
  gizmo_transform_origin(gizmo, &origin);
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
  float sensi = 0.1f; // sensi

  float delta = glm_signf(sign) * sensi;

  printf("x:%f,y:%f\t∆:%f\n", x, y, delta);

  vec3 movement;
  glm_vec3_scale(*camera_axis[axis], delta, movement);

  // move meshes
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_axis_add(list->entries[i], movement[axis], axis);

  // move gizmo
  gizmo_transform_translate_add(gizmo, movement[axis], axis);
}

void gizmo_transform_callback_translate(GizmoTransform *gizmo,
                                        MeshRefList *list, Camera *camera,
                                        Viewport *viewport) {

  const Axis axis = gizmo->axis;

  // draw raywast from mouse position
  Raycast mouse_ray;
  float x = g_input.mouse.x;
  float y = g_input.mouse.y;

  // project ray into axis
  vec3 target;
  // gizmo_transform_origin(gizmo, &target);
  glm_vec3_copy(gizmo->init_offset, target);

  vec3 axis_dir;
  vec_world_axis(axis, &axis_dir);

  vec3 position;
  raycast_project_from_screen_to_axis(
      &(RaycastProjectScreenToAxis){
          .origin = &camera->position,
          .target = &target,
          .axis_direction = &axis_dir,
          .view = &camera->view,
          .projection = &viewport->projection,
          .x = x,
          .y = y,
          .width = viewport->width,
          .height = viewport->height,
      },
      &position);

  glm_vec3_scale(position, -1.0f, position);
  print_vec3(position);

  // move meshes
  for (size_t i = 0; i < list->length; i++) 
    mesh_translate(list->entries[i], position);
  

  // move gizmo
  gizmo_transform_translate(gizmo, position);
}

void gizmo_transform_callback_rotate(GizmoTransform *gizmo, MeshRefList *list,
                                     Camera *camera, Viewport *viewport) {}

void gizmo_transform_callback_scale(GizmoTransform *gizmo, MeshRefList *list,
                                    Camera *camera, Viewport *viewport) {}
