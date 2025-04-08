#include "light.h"
#include "../utils/system.h"
#include "camera.h"
#include "emscripten/emscripten.h"
#include "shader.h"
#include "viewport.h"
#include <cglm/cglm.h>
#include <stdio.h>

/* =============================== SHADOW PROCESS ==============================
  Shadows use a Shadow Map approach. Meaning that they render multiple
  time the scene but under various view angles (each lights angles) to generate
  Depth Maps.
   1.Point light use a Cube Shadow Map: meaning that we will use
   our point light as a cube rendering 6 times ou scene with different angles
   2. For Directional light use Cascade Shadow Map

   To Generate the Depth Map we only require a Vertex Shader (no Fragment) as to
  only traslate the vertices under the light projection point of view

  We will then store those Depth Maps in each lights as TextureView
  and Sampler
  Once our Depth Map are stored we can finally use them in our "base" shaders

  Process Diagram:

                +----------------------+
                |        Light         |
                +----------------------+
                          |
                   Light Projection
                    (cube/cascade)
                          |
                  *****************
                  * Render pass 1 *
                  *****************
                          |
                  Generate Depth Map
                          |
                    Store Depth Map
                       Texture
                          |
                +----------------------+
                |        Mesh          |
                +----------------------+
                          |
                  Bind Depth Texture
                          |
                    Compare with
                      Fragment
                          |
                  *****************
                  * Render pass 2 *
                  *****************


  ===========================================================================

 */

void light_create_point(PointLight *light, PointLightDescriptor *desc) {

  // init to 0
  *light = (PointLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // copy color
  glm_vec3_copy(desc->color, light->color);
}

void light_create_directional(DirectionalLight *light,
                              DirectionalLightDescriptor *desc) {

  // init to 0
  *light = (DirectionalLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // copy target
  glm_vec3_copy(desc->target, light->target);

  // copy color
  glm_vec3_copy(desc->color, light->color);
}

void light_create_ambient(AmbientLight *light, AmbientLightDescriptor *desc) {

  // init to 0
  *light = (AmbientLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy color
  glm_vec3_copy(desc->color, light->color);
}

/**
   Compute point view for Point light
   Point lights use 6 views, each pointing to different directions
 */
LightViews light_point_views(vec3 light_position) {

  LightViews new_views = (LightViews){.length = LIGHT_POINT_VIEWS};

  vec3 directions[LIGHT_POINT_VIEWS] = {
      {1.0f, 0.0f, 0.0f},  // +x (right)
      {-1.0f, 0.0f, 0.0f}, // -x (left)
      {0.0f, 1.0f, 0.0f}, // +y (top)
      {0.0f, -1.0f, 0.0f},  // -y (bottom)
      {0.0f, 0.0f, 1.0f},  // +z (front)
      {0.0f, 0.0f, -1.0f}, // -z (back)
  };

  vec3 ups[LIGHT_POINT_VIEWS] = {
      {0.0f, 1.0f, 0.0f},  // +x (right)
      {0.0f, 1.0f, 0.0f},  // -x (left)
      {0.0f, 0.0f, 1.0f},  // +y (top)
      {0.0f, 0.0f, -1.0f}, // -y (bottom)
      {0.0f, 1.0f, 0.0f},  // +z (front)
      {0.0f, 1.0f, 0.0f},  // -z (back)
  };

  mat4 projection;
  glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, 100.0f, projection);

  for (int v = 0; v < new_views.length; v++) {
    vec3 direction;
    mat4 view;
    glm_vec3_add(light_position, directions[v], direction);
    glm_lookat(light_position, direction, ups[v], view);
    glm_mat4_mul(projection, view, new_views.views[v]);
  }

  return new_views;
}

/**
   Compute point view for directional light
   Directional Light work as the other way around compared to Point Light
   For DL as position agnostic, target is always 0,0,0, but the position
   simulates sun position by being super far away from the scene
 */
LightViews light_directional_view(vec3 light_position, vec3 light_target) {

  LightViews new_views = (LightViews){.length = LIGHT_DIRECTIONAL_VIEW};

  vec3 up = {0.0f, 1.0f, 0.0f};

  // adjust up if direction parallel to world up
  if (fabs(glm_vec3_dot(light_target, up)) > 0.99f)
    glm_vec3_copy((vec3){0.0f, 0.0f, 1.0f}, up);

  mat4 projection;
  glm_perspective(glm_rad(45.0f), 1.0f, 0.1f, 100.0f, projection);

  for (int v = 0; v < new_views.length; v++) {
    mat4 view;
    glm_lookat(light_position, light_target, up, view);
    glm_mat4_mul(projection, view, new_views.views[v]);
    print_mat4(new_views.views[v]);
  }

  /*float ortho_size = 50.0f;
  glm_ortho(-ortho_size, ortho_size, -ortho_size, ortho_size, 0.1f, 100.0f,
  projection);*/

  /*
  float distance = 70.0;
  vec3 direction = {-1.0f, -1.0f, -1.0f};
  glm_normalize(direction);
  glm_vec3_scale(direction, distance, direction);

  vec3 light_position;
  glm_vec3_sub(light_target, direction, light_position);
  print_vec3(light_position);
  */
  return new_views;
}
