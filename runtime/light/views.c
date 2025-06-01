#include "views.h"

/**
   Compute point view for Point light
   Point lights use 6 views, each pointing to different directions
 */
LightViews light_point_views(vec3 light_position, float near, float far) {

  LightViews new_views = (LightViews){.length = LIGHT_POINT_VIEWS};

  vec3 directions[LIGHT_POINT_VIEWS] = {
      {1.0f, 0.0f, 0.0f},  // +x (right)
      {-1.0f, 0.0f, 0.0f}, // -x (left)
      {0.0f, 1.0f, 0.0f},  // +y (top)
      {0.0f, -1.0f, 0.0f}, // -y (bottom)
      {0.0f, 0.0f, 1.0f},  // +z (front)
      {0.0f, 0.0f, -1.0f}, // -z (back)
  };

  vec3 ups[LIGHT_POINT_VIEWS] = {
      {0.0f, 1.0f, 0.0f},  // +x (right)
      {0.0f, 1.0f, 0.0f},  // -x (left)
      {0.0f, 0.0f, -1.0f}, // +y (top)
      {0.0f, 0.0f, 1.0f},  // -y (bottom)
      {0.0f, 1.0f, 0.0f},  // +z (front)
      {0.0f, 1.0f, 0.0f},  // -z (back)
  };

  mat4 projection;
  glm_perspective(glm_rad(90.0f), 1.0f, near, far, projection);

  /* Flipping projection X axis to match cubemap coordinates
     Probably has something to do with the fact that we see the cube maps
     from inside, so need to mirror its faces.
   */
  projection[0][0] *= -1.0f;

  for (int v = 0; v < new_views.length; v++) {

    vec3 direction;
    glm_vec3_add(light_position, directions[v], direction);

    mat4 view;
    glm_lookat(light_position, direction, ups[v], view);

    glm_mat4_mul(projection, view, new_views.views[v]);
  }

  return new_views;
}

/**
   Compute point view for spot light
 */
LightViews light_spot_view(vec3 light_position, vec3 light_target,
                           float angle) {

  LightViews new_views = (LightViews){.length = LIGHT_SPOT_VIEW};

  vec3 up = {0.0f, 1.0f, 0.0f};

  // adjust up if direction parallel to world up
  if (fabs(glm_vec3_dot(light_target, up)) > 0.99f)
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, up);

  mat4 projection;
  glm_perspective(glm_rad(angle), 1.0f, 0.1f, 100.0f, projection);

  for (int v = 0; v < new_views.length; v++) {
    mat4 view;
    glm_lookat(light_position, light_target, up, view);
    glm_mat4_mul(projection, view, new_views.views[v]);
  }

  return new_views;
}

/**
   Compute point view for sun light
   Sun Light work as the other way around compared to Point or Directionam Light
   For Sun are position agnostic, target is always 0,0,0, but the position
   simulates sun position by being super far away from the scene
 */
LightViews light_sun_view(vec3 light_position, float size) {

  LightViews new_views = (LightViews){.length = LIGHT_SPOT_VIEW};

  vec3 up = {0.0f, 1.0f, 0.0f};

  // adjust up if direction parallel to world up
  // if (fabs(glm_vec3_dot(light_position, up)) > 0.99f)
  // glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, up);

  mat4 ortho;
  glm_ortho(-size, size, -size, size, 0.1f, 100.0f, ortho);

  for (int v = 0; v < new_views.length; v++) {
    mat4 view;
    glm_lookat(light_position, (vec3){0.0f, 0.0f, 0.0f}, up, view);
    glm_mat4_mul(ortho, view, new_views.views[v]);
  }

  return new_views;
}

