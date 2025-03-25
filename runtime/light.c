#include "light.h"

void light_create_point(PointLight *light, PointLightDescriptor *desc) {

  // copy intensity
  *light = (PointLight){.intensity = desc->intensity};

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // copy color
  glm_vec3_copy(desc->color, light->color);
}

void light_create_directional(DirectionalLight *light,
                              DirectionalLightDescriptor *desc) {

  // copy intensity
  *light = (DirectionalLight){.intensity = desc->intensity};

  // copy position
  glm_vec3_copy(desc->position, light->position);

  // copy target
  glm_vec3_copy(desc->target, light->target);

  // copy color
  glm_vec3_copy(desc->color, light->color);
}

void light_create_ambient(AmbientLight *light, AmbientLightDescriptor *desc) {

  // copy intensity
  *light = (AmbientLight){.intensity = desc->intensity};

  // copy color
  glm_vec3_copy(desc->color, light->color);
}

/*
PointLightListUniform light_point_uniform(PointLightList *list) {}
DirectionalLightListUniform
light_directional_uniform(DirectionalLightList *list) {}
AmbientLightListUniform light_ambient_uniform(AmbientLightList *list) {}
*/
