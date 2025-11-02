#include "core.h"

#include <cglm/util.h>
#include <cglm/vec3.h>
#include <math.h>
#include <stdint.h>

#include "backend/registry.h"
#include "backend/ubo.h"
#include "uniform.h"
#include "utils/name.h"
#include "utils/projection.h"
#include "utils/system.h"

void point_light_create(PointLight *light, PointLightDescriptor *desc) {

  *light = (PointLight){0};
  point_light_set_name(light, desc->name == 0 ? "Point light" : desc->name);
  
  light->intensity = desc->intensity;
  light->cutoff = cos(glm_rad(desc->cutoff));
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));
  light->near = desc->near;
  light->far = desc->far;

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->color, light->color);
}

void spot_light_create(SpotLight *light, SpotLightDescriptor *desc) {

  *light = (SpotLight){0};
  spot_light_set_name(light, desc->name == 0 ? "Spot light" : desc->name);

  light->intensity = desc->intensity;
  light->cutoff = cos(glm_rad(desc->cutoff));
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));
  light->angle = desc->angle;

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->target, light->target);
  glm_vec3_copy(desc->color, light->color);
}

void sun_light_create(SunLight *light, SunLightDescriptor *desc) {

  *light = (SunLight){0};
  sun_light_set_name(light, desc->name == 0 ? "Sun light" : desc->name);

  light->intensity = desc->intensity;
  light->size = desc->size;

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->color, light->color);
}

void ambient_light_create(AmbientLight *light, AmbientLightDescriptor *desc) {

  *light = (AmbientLight){0};
  ambient_light_set_name(light, desc->name == 0 ? "Ambient light" : desc->name);

  light->intensity = desc->intensity;

  glm_vec4_copy(desc->color, light->color);
  glm_vec3_copy(desc->position, light->position);
}

// TODO
void point_light_destroy(PointLight *light) {}
void spot_light_destroy(SpotLight *light) {}
void ambient_light_destroy(AmbientLight *light) {}
void sun_light_destroy(SunLight *light) {}
