#include "callback.h"
#include "core.h"
#include "utils.h"

/*static const Axis rotate_axis[] = {
[Axis_X] = Axis
};*/

static inline void
gizmo_transform(GizmoTransform *gizmo, Camera *camera, Viewport *viewport,
                mesh_transform_axis_callback transform_callback, vec3 *delta);

// DELETEME
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

/**
   Generic function to transform gizmo based on axis and provided callback
   (trans/rot/scale)
 */
void gizmo_transform(GizmoTransform *gizmo, Camera *camera, Viewport *viewport,
                     mesh_transform_axis_callback transform_callback,
                     vec3 *delta) {

  const Axis axis = gizmo->axis;
  vec3 *gizmo_position = &gizmo->cache.gizmo_init_position;

  // draw raywast from mouse position
  Raycast mouse_ray;
  float x = g_input.mouse.x;
  float y = g_input.mouse.y;

  // project ray into axis
  vec3 axis_dir;
  vec_world_axis(axis, &axis_dir);

  vec3 projected_position;
  raycast_project_from_screen_to_axis(
      &(RaycastProjectScreenToAxis){
          .origin = &camera->position,
          .target = gizmo_position,
          .axis_direction = &axis_dir,
          .view = &camera->view,
          .projection = &viewport->projection,
          .x = x,
          .y = y,
          .width = viewport->width,
          .height = viewport->height,
      },
      &projected_position);

  vec3 gizmo_delta;
  // cancel initial offset
  glm_vec3_sub(projected_position, gizmo->cache.delta_init, gizmo_delta);

  // out
  if (delta)
    glm_vec3_copy(gizmo_delta, *delta);

  // move meshes
  for (size_t i = 0; i < gizmo->cache.selection.length; i++) {

    vec3 *init_position = &gizmo->cache.selection_init_attribute.entries[i];
    Mesh *mesh = gizmo->cache.selection.entries[i];

    // calculate offset
    vec3 offset_position;
    glm_vec3_add(*init_position, gizmo_delta, offset_position);

    // translate mesh
    transform_callback(mesh, offset_position[gizmo->axis], gizmo->axis);
  }
}

void gizmo_transform_callback_translate(GizmoTransform *gizmo, Camera *camera,
                                        Viewport *viewport) {

  vec3 delta;
  gizmo_transform(gizmo, camera, viewport, mesh_translate_axis, &delta);

  // translate gizmo
  vec3 gizmo_offset;
  glm_vec3_add(gizmo->cache.gizmo_init_position, delta, gizmo_offset);
  gizmo_transform_translate(gizmo, gizmo_offset);
}

void gizmo_transform_callback_rotate(GizmoTransform *gizmo, Camera *camera,
                                     Viewport *viewport) {
  printf("axis: %d\n", gizmo->axis);
  gizmo_transform(gizmo, camera, viewport, mesh_rotate_axis, NULL);
}

void gizmo_transform_callback_scale(GizmoTransform *gizmo, Camera *camera,
                                    Viewport *viewport) {
  gizmo_transform(gizmo, camera, viewport, mesh_scale_axis, NULL);
}
