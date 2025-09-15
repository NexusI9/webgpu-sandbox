#include "projection.h"

#include <cglm/cam.h>
#include <cglm/util.h>
#include <cglm/vec3.h>
#include <math.h>

#include "vector/core.h"
#include "../runtime/camera/core.h"
#include "../runtime/viewport/core.h"

const vec3 projection_cubemaps_directions[PROJECTION_VIEW_COUNT] = {
    {1.0f, 0.0f, 0.0f},  // +x (right)
    {-1.0f, 0.0f, 0.0f}, // -x (left)
    {0.0f, 1.0f, 0.0f},  // +y (top)
    {0.0f, -1.0f, 0.0f}, // -y (bottom)
    {0.0f, 0.0f, 1.0f},  // +z (front)
    {0.0f, 0.0f, -1.0f}, // -z (back)
};

const vec3 projection_cubemaps_ups[PROJECTION_VIEW_COUNT] = {
    {0.0f, 1.0f, 0.0f},  // +x (right)
    {0.0f, 1.0f, 0.0f},  // -x (left)
    {0.0f, 0.0f, -1.0f}, // +y (top)
    {0.0f, 0.0f, 1.0f},  // -y (bottom)
    {0.0f, 1.0f, 0.0f},  // +z (front)
    {0.0f, 1.0f, 0.0f},  // -z (back)
};

/**
   Compute point view for Point light
   Point lights use 6 views, each pointing to different directions
 */
void projection_point(Projection *views, const vec3 light_position,
                      const float near, const float far) {

  views->length = PROJECTION_VIEW_COUNT;

  mat4 projection;
  glm_perspective(glm_rad(90.0f), 1.0f, near, far, projection);

  /* Flipping projection X axis to match cubemap coordinates
     Probably has something to do with the fact that we see the cube maps
     from inside, so need to mirror its faces.
   */
  projection[0][0] *= -1.0f;

  glm_mat4_copy(projection, views->projection);

  for (int v = 0; v < views->length; v++) {

    vec3 direction;
    glm_vec3_add((float *)light_position,
                 (float *)projection_cubemaps_directions[v], direction);

    mat4 view;
    glm_lookat((float *)light_position, direction,
               (float *)projection_cubemaps_ups[v], view);

    glm_mat4_copy(view, views->views[v]);
    glm_mat4_mul(projection, view, views->combined[v]);
  }
}

/**
   Compute point view for spot light
 */
void projection_spot(Projection *views, const vec3 light_position,
                     const vec3 light_target, const float angle) {

  views->length = 1;

  vec3 up = {0.0f, 1.0f, 0.0f};

  // adjust up if direction parallel to world up
  if (fabs(glm_vec3_dot((float *)light_target, up)) > 0.99f)
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, up);

  mat4 projection;
  glm_perspective(glm_rad(angle), 1.0f, 0.1f, 100.0f, projection);

  glm_mat4_copy(projection, views->projection);

  for (int v = 0; v < views->length; v++) {
    mat4 view;
    glm_lookat((float *)light_position, (float *)light_target, up, view);

    glm_mat4_copy(view, views->views[v]);
    glm_mat4_mul(projection, view, views->combined[v]);
  }
}

/**
   Compute point view for sun light
   Sun Light work as the other way around compared to Point or Directionam Light
   For Sun are position agnostic, target is always 0,0,0, but the position
   simulates sun position by being super far away from the scene
 */
void projection_sun(Projection *views, const vec3 light_position,
                    const float size) {

  views->length = 1;

  vec3 up = {0.0f, 1.0f, 0.0f};

  // adjust up if direction parallel to world up
  // if (fabs(glm_vec3_dot(light_position, up)) > 0.99f)
  // glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, up);

  // For sun: normalize position and set it far away by default
  vec3 norm_position, view_position;
  glm_vec3_normalize_to((float *)light_position, norm_position);
  glm_vec3_scale(norm_position, (float)PROJECTION_SUN_DISTANCE, view_position);

  mat4 ortho;
  glm_ortho(-size, size, -size, size, 0.1f, 100.0f, ortho);

  glm_mat4_copy(ortho, views->projection);

  for (int v = 0; v < views->length; v++) {
    mat4 view;
    glm_lookat(view_position, (vec3){0.0f, 0.0f, 0.0f}, up, view);

    glm_mat4_copy(view, views->views[v]);
    glm_mat4_mul(ortho, view, views->combined[v]);
  }
}

void projection_mirror(Projection *projection, const vec3 n, const float d,
                       const Camera *camera, const Viewport *viewport) {

  projection->length = 1;

  // compute mirrored view
  vec3 mir_pos, mir_fwd, mir_up, mir_target;
  /*
   cast to float* to pass as const
   cf: https://www.open-std.org/jtc1/sc22/wg14/www/docs/n2607.pdf
  */
  vec3_reflect_point((float *)camera->position, (float *)n, d, mir_pos);
  vec3_reflect_dir((float *)camera->forward, (float *)n, mir_fwd);
  vec3_reflect_dir((float *)camera->up, (float *)n, mir_up);

  glm_vec3_add(mir_pos, mir_fwd, mir_target);

  glm_lookat(mir_pos, mir_target, mir_up, projection->views[0]);

  // copy initial viewport
  glm_mat4_copy((float(*)[])viewport->projection, projection->projection);

  // create combined view
  glm_mat4_mul(projection->projection, projection->views[0],
               projection->combined[0]);
}
