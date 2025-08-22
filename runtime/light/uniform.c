#include "uniform.h"
#include "list.h"
#include <string.h>
#include "../utils/projection.h"

static inline bool light_comparator_different(const LightComparator *,
                                              const LightComparator *);

/* Comparator */

bool light_comparator_different(const LightComparator *a,
                                const LightComparator *b) {
  return memcmp(a, b, sizeof(LightComparator)) != 0;
}

/* Uniforms */
void point_light_uniform(PointLightUniform *uniform, PointLight *light) {

  *uniform = (PointLightUniform){0};
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
  
  for (uint8_t v = 0; v < LIGHT_POINT_VIEWS; v++)
    glm_mat4_copy(points_views.views[v], uniform->views[v]);
}

void ambient_light_uniform(AmbientLightUniform *uniform, AmbientLight *light) {

  // map light to light uniform (including paddings...)
  *uniform = (AmbientLightUniform){0};
  uniform->intensity = light->intensity;
  glm_vec3_copy(light->color, uniform->color);
}

void spot_light_uniform(SpotLightUniform *uniform, SpotLight *light) {

  *uniform = (SpotLightUniform){0};
  uniform->intensity = light->intensity;
  uniform->cutoff = light->cutoff;
  uniform->inner_cutoff = light->inner_cutoff;
  glm_vec3_copy(light->color, uniform->color);
  glm_vec3_copy(light->target, uniform->target);
  glm_vec3_copy(light->position, uniform->position);

  // get light view matrix
  Projection spot_view;
  projection_spot(&spot_view, light->position, light->target, light->angle);

  glm_mat4_copy(spot_view.views[0], uniform->view);
}

void sun_light_uniform(SunLightUniform *uniform, SunLight *light) {

  *uniform = (SunLightUniform){0};
  uniform->intensity = light->intensity;
  glm_vec3_copy(light->position, uniform->position);
  glm_vec3_copy(light->color, uniform->color);

  // get light view matrix
  Projection sun_view;
  projection_sun(&sun_view, light->position, light->size);

  glm_mat4_copy(sun_view.views[0], uniform->view);
}

/* Callbacks */

/**
   Map the light into its "uniform version" and replace the entry_data directly
 */
void point_light_list_update_callback(void *callback_data, void *entry_data) {

  PointLightListBase *list = (PointLightListBase *)callback_data;
  PointLightListUniform *uniform = (PointLightListUniform *)entry_data;

  for (size_t i = 0; i < list->length; i++)
    point_light_uniform(&uniform->entries[i], &list->entries[i]);
}

/**
   Compare the each point light from the list with the one from the uniform
 */
bool point_light_list_trigger_callback(void *callback_data,
                                       const void *entry_data) {

  PointLightListBase *list = (PointLightListBase *)callback_data;
  PointLightListUniform *unif = (PointLightListUniform *)entry_data;

  for (size_t i = 0; i < list->length; list++)
    if (light_comparator_different(
            &(LightComparator){
                .position =
                    {
                        list->entries[i].position[0],
                        list->entries[i].position[1],
                        list->entries[i].position[2],
                    },
            },
            &(LightComparator){
                .position =
                    {
                        unif->entries[i].position[0],
                        unif->entries[i].position[1],
                        unif->entries[i].position[2],
                    },
            }))

      return true;

  return false;
}

void ambient_light_list_update_callback(void *callback_data, void *entry_data) {

}
bool ambient_light_list_trigger_callback(void *callback_data,
                                         const void *entry_data) {
  return false;
}

void spot_light_list_update_callback(void *callback_data, void *entry_data) {}

bool spot_light_list_trigger_callback(void *callback_data,
                                      const void *entry_data) {
  return false;
}

void sun_light_list_update_callback(void *callback_data, void *entry_data) {}

bool sun_light_list_trigger_callback(void *callback_data,
                                     const void *entry_data) {
  return false;
}
