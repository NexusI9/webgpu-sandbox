#include "uniform.h"
#include "../utils/projection.h"
#include "core.h"
#include "list.h"
#include <string.h>

/* Uniforms */
void light_point_uniform_update(PointLight *light) {

  PointLightUniform *uniform = light->ssbo_slot[LightSSBOSlot_List].uniform;
  uniform->intensity = light->intensity;
  uniform->cutoff = light->cutoff;
  uniform->inner_cutoff = light->inner_cutoff;
  uniform->near = light->near;
  uniform->far = light->far;

  glm_vec3_copy(light->color, uniform->color);
  glm_vec3_copy(light->position, uniform->position);

  // copy 6 points views for shader depth comparison
  Projection points_views;
  projection_point(&points_views, light->position, light->near, light->far);

  for (uint8_t v = 0; v < points_views.length; v++)
    glm_mat4_copy(points_views.combined[v], uniform->views[v]);
}

void light_ambient_uniform_update(AmbientLight *light) {

  // map light to light uniform (including paddings...)
  AmbientLightUniform *uniform = light->ssbo_slot.uniform;
  uniform->intensity = light->intensity;
  glm_vec3_copy(light->color, uniform->color);
}

void light_spot_uniform_update(SpotLight *light) {

  SpotLightUniform *uniform = light->ssbo_slot[LightSSBOSlot_List].uniform;

  uniform->intensity = light->intensity;
  uniform->cutoff = light->cutoff;
  uniform->inner_cutoff = light->inner_cutoff;
  glm_vec3_copy(light->color, uniform->color);
  glm_vec3_copy(light->target, uniform->target);
  glm_vec3_copy(light->position, uniform->position);

  // get light view matrix
  Projection spot_view;
  projection_spot(&spot_view, light->position, light->target, light->angle);

  for (uint8_t v = 0; v < spot_view.length; v++)
    glm_mat4_copy(spot_view.combined[v], uniform->view);
}

void light_sun_uniform_update(SunLight *light) {

  SunLightUniform *uniform = light->ssbo_slot[LightSSBOSlot_List].uniform;

  uniform->intensity = light->intensity;
  glm_vec3_copy(light->position, uniform->position);
  glm_vec3_copy(light->color, uniform->color);

  // get light view matrix
  Projection sun_view;
  projection_sun(&sun_view, light->position, light->size);

  for (uint8_t v = 0; v < sun_view.length; v++)
    glm_mat4_copy(sun_view.combined[v], uniform->view);
}
