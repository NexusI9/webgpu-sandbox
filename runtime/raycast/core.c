#include "core.h"
#include "../input/input.h"
#include <float.h>
#include <math.h>

bool raycast_hit_aabb(Raycast *ray, const AABB *box, float *distance) {

  float tmin = -FLT_MAX, tmax = FLT_MAX;

  for (int i = 0; i < 3; ++i) {

    if (fabs(ray->direction[i]) < 1e-6f) {
      // ray parallel to slab
      if (ray->origin[i] < box->min[i] || ray->origin[i] > box->max[i])
        return false;
    } else {
      // precompute division (faster)
      float inv_d = 1.0f / ray->direction[i];

      float t1 = (box->min[i] - ray->origin[i]) * inv_d;
      float t2 = (box->max[i] - ray->origin[i]) * inv_d;

      if (t1 > t2) {
        float tmp = t1;
        t1 = t2;
        t2 = tmp;
      }

      if (t1 > tmin)
        tmin = t1;

      if (t2 < tmax)
        tmax = t2;

      // printf("[%d] tmin: %f, tmax: %f\n", i, tmin, tmax);
      if (tmin > tmax)
        return false;
    }
  }

  if (distance)
    *distance = tmin;

  return true;
}

/**
   Cast ray from a given mouse position to world space
 */
void raycast_from_screen(Raycast *ray, vec3 *origin, mat4 *view,
                         mat4 *projection, float x, float y) {

  // near plane point in clip space
  vec4 ray_clip = {x, y, -1.0f, 1.0f};

  // unproject to world space
  mat4 inv_proj, inv_view;

  // TODO: cache inverted matrix
  glm_mat4_inv(*projection, inv_proj);
  glm_mat4_inv(*view, inv_view);

  // eye space (remove projection)
  vec4 ray_eye;
  glm_mat4_mulv(inv_proj, ray_clip, ray_eye);
  // direction in eye space
  ray_eye[2] = -1.0f;
  ray_eye[3] = 0.0f;

  // world space (remove view)
  vec4 ray_world;
  glm_mat4_mulv(inv_view, ray_eye, ray_world);
  vec3 ray_dir = {ray_world[0], ray_world[1], ray_world[2]};
  glm_vec3_normalize(ray_dir);

  // set origin
  glm_vec3_zero(ray->origin);
  glm_vec3_copy(*origin, ray->origin);

  // set direction
  glm_vec3_zero(ray->direction);
  glm_vec3_copy(ray_dir, ray->direction);
}

void raycast_project_to_axis(Raycast *ray, vec3 *position, vec3 *axis_direction,
                             vec3 *dest) {
  vec3 q = {0}, p = {0};
  glm_vec3_copy(ray->origin, q);
  glm_vec3_copy(*position, p);

  vec3 d1, d2; // ray_dir, axis_dir
  glm_vec3_copy(ray->direction, d1);
  glm_vec3_copy(*axis_direction, d2);

  vec3 r;
  glm_vec3_sub(q, p, r);

  // get closest point to axis
  float d1_dot_d1 = glm_vec3_dot(d1, d1);
  float d1_dot_d2 = glm_vec3_dot(d1, d2);
  float d2_dot_d2 = glm_vec3_dot(d2, d2);
  float r_dot_d1 = glm_vec3_dot(r, d1);
  float r_dot_d2 = glm_vec3_dot(r, d2);

  float denom = d1_dot_d1 * d2_dot_d2 - d1_dot_d2 * d1_dot_d2;

  if (fabsf(denom) < 1e-6f) {
    glm_vec3_copy(*position, *dest); // fallback
    return;
  }

  float t = (r_dot_d1 * d1_dot_d2 - r_dot_d2 * d1_dot_d1) / denom;

  vec3 move_pos;
  glm_vec3_scale(d2, t, move_pos);
  glm_vec3_sub(*position, move_pos, *dest);
}

/**
   Convert a mouse projection to world space and project it onto a specific
   axis.

   Destination returns the position of the closest point on the
   axis based on the initial mouse position.

   Used for gizmo transform to move the objects accordingly based on the
   selected axis.
 */
void raycast_project_from_screen_to_axis(Raycast *ray,
                                         const RaycastProjectScreenToAxis *desc,
                                         vec3 *dest) {

  // convert mouse to ndc (-1/1)
  float x, y;
  input_mouse_NDC(desc->x, desc->y, desc->width, desc->height, &x, &y);

  raycast_from_screen(ray, desc->origin, desc->view, desc->projection, x, y);

  raycast_project_to_axis(ray, desc->target, desc->axis_direction, dest);
}

/**
   Convert a mouse projection to world space and project it onto an infinite
   plane with a defined normal direction.

   Destination returns the position of the closest point on the
   axis based on the initial mouse position.

   Used for gizmo transform to move the objects accordingly based on the
   selected plane (XY, YZ, XZ, Camera).
 */
void raycast_project_from_screen_to_plane(
    Raycast *ray, const RaycastProjectScreenToAxis *desc, vec3 *dest) {

  // convert mouse to ndc (-1/1)
  // retrieve mouse position
  float x, y;
  input_mouse_NDC(g_input.mouse.x, g_input.mouse.y, desc->width, desc->height,
                  &x, &y);

  // cast ray from mouse to world
  Raycast raycast;
  raycast_from_screen(&raycast, desc->origin, desc->view, desc->projection, x,
                      y);

  // create infinite plane based on axis direction and target
  raycast_hit_inf_plane(&raycast, desc->plane, dest);
}

/**
   Detect if raycast hit an infinite plane
 */
bool raycast_hit_inf_plane(Raycast *ray, InfinitePlane *plane, vec3 *dest) {

  float denom = glm_vec3_dot(plane->normal, ray->direction);

  // ~ 0: parallel to plane
  if (fabsf(denom) < 1e-6f)
    return false;

  float t = -(glm_vec3_dot(plane->normal, ray->origin) + plane->d) / denom;

  // intersection behind ray origin
  if (t < 0.0f)
    return false;

  vec3 scaled_dir;
  glm_vec3_scale(ray->direction, t, scaled_dir);
  glm_vec3_add(ray->origin, scaled_dir, *dest);

  return true;
}

/**
   Project the mouse towards a certain axis (1D or 2D) in world space.
   The function basically dispatch to two sub-methods : .._to_axis or
   .._to_plane depending on the dimension of the Axis (1D or 2D).
 */
void raycast_project_from_screen(Raycast *raycast, const Axis axis,
                                 RaycastProjectScreenToAxis *desc, vec3 *dest) {
  switch (axis) {

    // 1D => project to Axis
  case Axis_X:
  case Axis_Y:
  case Axis_Z:
    raycast_project_from_screen_to_axis(raycast, desc, dest);
    break;

    // 2D => project to Plane
  case Axis_XY:
  case Axis_YZ:
  case Axis_XZ:
  case Axis_XYZ:
  case Axis_View: {

    // create plane from axis direction
    InfinitePlane plane;
    inf_plane_create(&plane, *desc->target, *desc->axis_direction);
    desc->plane = &plane;

    raycast_project_from_screen_to_plane(raycast, desc, dest);
    break;
  }
  }
}
