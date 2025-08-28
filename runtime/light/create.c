#include "create.h"
#include "uniform.h"
#include <stdlib.h>

void light_create_point(PointLight *light, PointLightDescriptor *desc) {

  *light = (PointLight){0};

  light->id = reg_register((void *)light, RegEntryType_PointLight);
  light->intensity = desc->intensity;
  light->cutoff = cos(glm_rad(desc->cutoff));
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));
  light->near = desc->near;
  light->far = desc->far;

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->color, light->color);

  light->ssbo_slot.uniform = malloc(sizeof(PointLightUniform));
  point_light_uniform_update(light);
}

void light_create_spot(SpotLight *light, SpotLightDescriptor *desc) {

  *light = (SpotLight){0};

  light->intensity = desc->intensity;
  light->cutoff = cos(glm_rad(desc->cutoff));
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));
  light->angle = desc->angle;
  light->id = reg_register((void *)light, RegEntryType_SpotLight);

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->target, light->target);
  glm_vec3_copy(desc->color, light->color);

  light->ssbo_slot.uniform = malloc(sizeof(SpotLightUniform));
  spot_light_uniform_update(light);
}

void light_create_ambient(AmbientLight *light, AmbientLightDescriptor *desc) {

  *light = (AmbientLight){0};

  light->id = reg_register((void *)light, RegEntryType_AmbientLight);
  light->intensity = desc->intensity;

  glm_vec3_copy(desc->color, light->color);
  glm_vec3_copy(desc->position, light->position);

  light->ssbo_slot.uniform = malloc(sizeof(AmbientLightUniform));
  ambient_light_uniform_update(light);
}

void light_create_sun(SunLight *light, SunLightDescriptor *desc) {

  *light = (SunLight){0};

  light->id = reg_register((void *)light, RegEntryType_SunLight);
  light->intensity = desc->intensity;
  light->size = desc->size;

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->color, light->color);

  light->ssbo_slot.uniform = malloc(sizeof(SunLightUniform));
  sun_light_uniform_update(light);
}
