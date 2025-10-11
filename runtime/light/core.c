#include "core.h"

#include <cglm/util.h>
#include <cglm/vec3.h>
#include <math.h>
#include <stdint.h>

#include "backend/registry.h"
#include "backend/ssbo.h"
#include "uniform.h"
#include "utils/name.h"
#include "utils/projection.h"
#include "utils/system.h"

void light_point_create(PointLight *light, PointLightDescriptor *desc) {

  *light = (PointLight){0};
  name_copy(desc->name == 0 ? "Point light" : desc->name, light->name);

  light->id = reg_register((void *)light, RegEntryType_PointLight);
  light->intensity = desc->intensity;
  light->cutoff = cos(glm_rad(desc->cutoff));
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));
  light->near = desc->near;
  light->far = desc->far;

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->color, light->color);

  /* === Init SSBO ===*/

  ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_List],
                       sizeof(PointLightUniform));

  light_point_uniform_update(light);

  for (uint8_t i = 0; i < PROJECTION_VIEW_COUNT; i++)
    ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_View + i],
                         sizeof(ProjectionUniform));

  light_point_projection_update(light);
}

void light_spot_create(SpotLight *light, SpotLightDescriptor *desc) {

  *light = (SpotLight){0};
  name_copy(desc->name == 0 ? "Spot light" : desc->name, light->name);

  light->intensity = desc->intensity;
  light->cutoff = cos(glm_rad(desc->cutoff));
  light->inner_cutoff = cos(glm_rad(desc->inner_cutoff));
  light->angle = desc->angle;
  light->id = reg_register((void *)light, RegEntryType_SpotLight);

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->target, light->target);
  glm_vec3_copy(desc->color, light->color);

  /* === Init SSBO ===*/

  ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_List],
                       sizeof(SpotLightUniform));
  light_spot_uniform_update(light);

  ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_View],
                       sizeof(ProjectionUniform));

  light_spot_projection_update(light);
}

void light_sun_create(SunLight *light, SunLightDescriptor *desc) {

  *light = (SunLight){0};
  name_copy(desc->name == 0 ? "Sun light" : desc->name, light->name);

  light->id = reg_register((void *)light, RegEntryType_SunLight);
  light->intensity = desc->intensity;
  light->size = desc->size;

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->color, light->color);

  /* === Init SSBO ===*/

  ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_List],
                       sizeof(SunLightUniform));

  light_sun_uniform_update(light);

  ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_View],
                       sizeof(ProjectionUniform));

  light_sun_projection_update(light);
}

void light_ambient_create(AmbientLight *light, AmbientLightDescriptor *desc) {

  *light = (AmbientLight){0};
  name_copy(desc->name == 0 ? "Ambient light" : desc->name, light->name);

  light->id = reg_register((void *)light, RegEntryType_AmbientLight);
  light->intensity = desc->intensity;

  glm_vec4_copy(desc->color, light->color);
  glm_vec3_copy(desc->position, light->position);
  
  /* === Init SSBO ===*/

  ssbo_slot_init_alloc(&light->ssbo_slot, sizeof(AmbientLightUniform));

  light_ambient_uniform_update(light);
}
