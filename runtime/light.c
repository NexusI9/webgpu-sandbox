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

void static light_point_shadow_map(PointLight *);

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

void light_point_shadow_map(PointLight *light) {
  /*
    CUBE MAP SHADOW
    Point lights use 6 views, each pointing to different directions
   */
}

PointLightViews light_point_views(vec3 light_position, viewport *vp) {

  PointLightViews new_views = (PointLightViews){
      .length = LIGHT_POINT_VIEWS,
  };

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
