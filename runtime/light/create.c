#include "create.h"

void light_create_point(PointLight *light, PointLightDescriptor *desc) {

  // init to 0
  *light = (PointLight){0};

  // copy intensity (exponent)
  light->intensity = desc->intensity;

  // copy cutoff
  light->cutoff = cos(glm_rad(desc->cutoff));

  // copy inner cutoff
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));

  // copy near plane
  light->near = desc->near;

  // copy far plane
  light->far = desc->far;

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // copy color
  glm_vec3_copy(desc->color, light->color);

  // set id
  light->id = gen_id();
}

void light_create_spot(SpotLight *light, SpotLightDescriptor *desc) {

  // init to 0
  *light = (SpotLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // copy target
  glm_vec3_copy(desc->target, light->target);

  // copy color
  glm_vec3_copy(desc->color, light->color);

  // copy cutoff and convert to cosinus radians
  light->cutoff = cos(glm_rad(desc->cutoff));
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));

  // copy angle
  light->angle = desc->angle;

  // set id
  light->id = gen_id();
}

void light_create_ambient(AmbientLight *light, AmbientLightDescriptor *desc) {

  // init to 0
  *light = (AmbientLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy color
  glm_vec3_copy(desc->color, light->color);

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // set id
  light->id = gen_id();
}

void light_create_sun(SunLight *light, SunLightDescriptor *desc) {

  // init to 0
  *light = (SunLight){0};

  // copy intensity
  light->intensity = desc->intensity;

  // copy size
  light->size = desc->size;

  // copy position
  // For sun: normalize position and set it far away by default
  float distance = (float)LIGHT_SUN_DISTANCE;
  vec3 new_position;
  glm_vec3_norm(desc->position);
  glm_vec3_scale(desc->position, distance, new_position);
  glm_vec3_copy(new_position, light->position);

  // copy color
  glm_vec3_copy(desc->color, light->color);

  // set id
  light->id = gen_id();
}
