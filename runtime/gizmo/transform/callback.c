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
static inline void gizmo_transform_axis(GizmoTransform *, Camera *, Viewport *,
                                        mesh_transform_axis_callback, vec3 *);

static inline void gizmo_transform_angle(GizmoTransform *, Camera *, Viewport *,
                                         mesh_transform_axis_callback, vec3 *);

/**
   Generic function to transform gizmo based on axis and provided callback
   (trans/rot/scale)
 */
void gizmo_transform_axis(GizmoTransform *gizmo, Camera *camera,
                          Viewport *viewport,
                          mesh_transform_axis_callback transform_callback,
                          vec3 *delta) {

  vec3 *gizmo_position = &gizmo->cache.gizmo_init_position;

  // draw raywast from mouse position
  Raycast mouse_ray;

  vec3 projected_position;
  raycast_project_from_screen(
      &mouse_ray, gizmo->axis,
      &(RaycastProjectScreenToAxis){
          .origin = &camera->position,
          .target = gizmo_position,
          .axis_direction = &gizmo->cache.axis_direction,
          .view = &camera->view,
          .projection = &viewport->projection,
          .x = g_input.mouse.x,
          .y = g_input.mouse.y,
          .width = viewport->width,
          .height = viewport->height,
      },
      &projected_position);

  vec3 gizmo_delta;

  // EDGE CASE: if scaling on all axis => uniform scale
  if (gizmo->mode == GizmoTransformMode_Scale && gizmo->axis == Axis_XYZ) {

    // calculate offset distance and replace delta
    float dist =
        glm_vec3_distance(gizmo->cache.gizmo_init_position, projected_position);
    float delta_dist =
        (dist - gizmo->cache.init_distance) * gizmo->cache.init_inv_distance;

    glm_vec3_copy((vec3){delta_dist, delta_dist, delta_dist}, gizmo_delta);
  } else {
    // cancel initial offset
    glm_vec3_sub(projected_position, gizmo->cache.init_delta, gizmo_delta);
  }

  // out
  if (delta)
    glm_vec3_copy(gizmo_delta, *delta);

}

/**
   Project a plane orthogonal to the active axis and calculate
 */
void gizmo_transform_angle(GizmoTransform *gizmo, Camera *camera,
                           Viewport *viewport,
                           mesh_transform_axis_callback transform_callback,
                           vec3 *dest) {

  // cast ray from mouse to world
  Raycast raycast;
  vec3 hit_position;

  raycast_project_from_screen_to_plane(&raycast,
                                       &(RaycastProjectScreenToAxis){
                                           .origin = &camera->position,
                                           .plane = &gizmo->cache.plane,
                                           .view = &camera->view,
                                           .projection = &viewport->projection,
                                           .x = g_input.mouse.x,
                                           .y = g_input.mouse.y,
                                           .width = viewport->width,
                                           .height = viewport->height,
                                       },
                                       &hit_position);

  // get vector from center to these points
  vec3 v0, v1;
  glm_vec3_sub(gizmo->cache.init_delta, gizmo->cache.gizmo_init_position, v0);
  glm_vec3_normalize(v0);

  glm_vec3_sub(hit_position, gizmo->cache.gizmo_init_position, v1);
  glm_vec3_normalize(v1);

  // get angle between vectors around axis
  float angle = glm_deg(acosf(glm_vec3_dot(v0, v1)));

  // get sign using cross product
  vec3 cross;
  glm_vec3_cross(v0, v1, cross);
  float sign = glm_signf(glm_vec3_dot(cross, gizmo->cache.axis_direction));

  angle *= sign;

  vec3 rotation;
  glm_vec3_scale(gizmo->cache.axis_direction, angle, rotation);

  if (dest)
    glm_vec3_copy(rotation, *dest);
}

/**
    ▗▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖ ▗▄▄▖
   ▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘▐▌
   ▐▌   ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖  ▝▀▚▖
   ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘


   Gizmo transforms callback are mostly use to project the curso to world space
   and calculate the delta depending on the given axis.

   Those callback do not handle the mesh selection transform. This process is
   handled in the scene selection draw callback.

 */
void gizmo_transform_callback_translate(GizmoTransform *gizmo, Camera *camera,
                                        Viewport *viewport, vec3 *delta) {

  // transform selection
  gizmo_transform_axis(gizmo, camera, viewport, mesh_translate_axis, delta);

  // translate gizmo based on cached delta
  vec3 gizmo_offset;
  glm_vec3_add(gizmo->cache.gizmo_init_position, *delta, gizmo_offset);
  gizmo_transform_translate(gizmo, gizmo_offset);
}

void gizmo_transform_callback_rotate(GizmoTransform *gizmo, Camera *camera,
                                     Viewport *viewport, vec3 *delta) {

  gizmo_transform_angle(gizmo, camera, viewport, mesh_rotate_axis, delta);
}

void gizmo_transform_callback_scale(GizmoTransform *gizmo, Camera *camera,
                                    Viewport *viewport, vec3 *delta) {

  gizmo_transform_axis(gizmo, camera, viewport, mesh_scale_axis, delta);
}
