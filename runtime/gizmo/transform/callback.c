#include "callback.h"
#include "core.h"
#include "utils.h"

/**
   ▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▄  ▗▄▄▖
   ▐▛▚▞▜▌▐▌     █  ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌
   ▐▌  ▐▌▐▛▀▀▘  █  ▐▛▀▜▌▐▌ ▐▌▐▌  █ ▝▀▚▖
   ▐▌  ▐▌▐▙▄▄▖  █  ▐▌ ▐▌▝▚▄▞▘▐▙▄▄▀▗▄▄▞▘

   Utilities function to retrieve the delta or angle
 */
static inline void
gizmo_transform(GizmoTransform *gizmo, Camera *camera, Viewport *viewport,
                mesh_transform_axis_callback transform_callback, vec3 *delta);

static inline void
gizmo_transform_angle(GizmoTransform *gizmo, Camera *camera, Viewport *viewport,
                      mesh_transform_axis_callback transform_callback,
                      vec3 *delta);

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
  raycast_project_from_screen_to_axis(&mouse_ray,
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

    vec3 *init_attribute = &gizmo->cache.selection_init_attribute.entries[i];
    Mesh *mesh = gizmo->cache.selection.entries[i];

    // calculate offset
    vec3 offset_attribute;

    glm_vec3_add(*init_attribute, gizmo_delta, offset_attribute);

    // translate mesh
    transform_callback(mesh, offset_attribute[gizmo->axis], gizmo->axis);
  }
}

/**
   Project a plane orthogonal to the active axis and calculate
 */
void gizmo_transform_angle(GizmoTransform *gizmo, Camera *camera,
                           Viewport *viewport,
                           mesh_transform_axis_callback transform_callback,
                           vec3 *dest) {

  // retrieve axis direction
  const Axis axis = gizmo->axis;
  vec3 axis_dir;
  vec_world_axis(axis, &axis_dir);

  // retrieve mouse position
  float x, y;
  input_mouse_NDC(g_input.mouse.x, g_input.mouse.y, viewport->width,
                  viewport->height, &x, &y);

  // cast ray from mouse to world
  Raycast raycast;
  raycast_from_screen(&raycast, &camera->position, &camera->view,
                      &viewport->projection, x, y);

  vec3 hit_position;
  raycast_hit_inf_plane(&raycast, &gizmo->cache.plane, &hit_position);

  // get vector from center to these points
  vec3 v0, v1;
  glm_vec3_sub(gizmo->cache.delta_init, gizmo->cache.gizmo_init_position, v0);
  glm_vec3_normalize(v0);

  glm_vec3_sub(hit_position, gizmo->cache.gizmo_init_position, v1);
  glm_vec3_normalize(v1);

  // get angle between vectors around axis
  float angle = glm_deg(acosf(glm_vec3_dot(v0, v1)));

  // get sign using cross product
  vec3 cross;
  glm_vec3_cross(v0, v1, cross);
  float sign = glm_signf(glm_vec3_dot(cross, axis_dir));

  angle *= sign;

  vec3 rotation;
  glm_vec3_scale(axis_dir, angle, rotation);
  // move meshes
  for (size_t i = 0; i < gizmo->cache.selection.length; i++) {

    vec3 *init_attribute = &gizmo->cache.selection_init_attribute.entries[i];
    Mesh *mesh = gizmo->cache.selection.entries[i];

    // calculate offset
    vec3 offset_attribute;
    glm_vec3_add(*init_attribute, rotation, offset_attribute);

    // translate mesh
    transform_callback(mesh, offset_attribute[gizmo->axis], gizmo->axis);
  }
}

/**
    ▗▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖ ▗▄▄▖
   ▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘▐▌
   ▐▌   ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖  ▝▀▚▖
   ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘

 */
void gizmo_transform_callback_translate(GizmoTransform *gizmo, Camera *camera,
                                        Viewport *viewport) {

  vec3 delta;

  // transform selection
  gizmo_transform(gizmo, camera, viewport, mesh_translate_axis, &delta);

  // translate gizmo based on cached delta
  vec3 gizmo_offset;
  glm_vec3_add(gizmo->cache.gizmo_init_position, delta, gizmo_offset);
  gizmo_transform_translate(gizmo, gizmo_offset);
}

void gizmo_transform_callback_rotate(GizmoTransform *gizmo, Camera *camera,
                                     Viewport *viewport) {
  gizmo_transform_angle(gizmo, camera, viewport, mesh_rotate_axis, NULL);
}

void gizmo_transform_callback_scale(GizmoTransform *gizmo, Camera *camera,
                                    Viewport *viewport) {
  gizmo_transform(gizmo, camera, viewport, mesh_scale_axis, NULL);
}
