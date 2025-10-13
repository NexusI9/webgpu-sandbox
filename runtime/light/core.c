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

void point_light_create(PointLight *light, PointLightDescriptor *desc) {

  *light = (PointLight){0};
  point_light_set_name(light, desc->name == 0 ? "Point light" : desc->name);

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

  point_light_uniform_update(light);

  for (uint8_t i = 0; i < PROJECTION_VIEW_COUNT; i++)
    ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_View + i],
                         sizeof(ProjectionUniform));

  point_light_projection_update(light);
}

void spot_light_create(SpotLight *light, SpotLightDescriptor *desc) {

  *light = (SpotLight){0};
  spot_light_set_name(light, desc->name == 0 ? "Spot light" : desc->name);

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
  spot_light_uniform_update(light);

  ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_View],
                       sizeof(ProjectionUniform));

  spot_light_projection_update(light);
}

void sun_light_create(SunLight *light, SunLightDescriptor *desc) {

  *light = (SunLight){0};
  sun_light_set_name(light, desc->name == 0 ? "Sun light" : desc->name);

  light->id = reg_register((void *)light, RegEntryType_SunLight);
  light->intensity = desc->intensity;
  light->size = desc->size;

  glm_vec3_copy(desc->position, light->position);
  glm_vec3_copy(desc->color, light->color);

  /* === Init SSBO ===*/

  ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_List],
                       sizeof(SunLightUniform));

  sun_light_uniform_update(light);

  ssbo_slot_init_alloc(&light->ssbo_slot[LightSSBOSlot_View],
                       sizeof(ProjectionUniform));

  sun_light_projection_update(light);
}

void ambient_light_create(AmbientLight *light, AmbientLightDescriptor *desc) {

  *light = (AmbientLight){0};
  ambient_light_set_name(light, desc->name == 0 ? "Ambient light" : desc->name);

  light->id = reg_register((void *)light, RegEntryType_AmbientLight);
  light->intensity = desc->intensity;

  glm_vec4_copy(desc->color, light->color);
  glm_vec3_copy(desc->position, light->position);

  /* === Init SSBO ===*/

  ssbo_slot_init_alloc(&light->ssbo_slot, sizeof(AmbientLightUniform));

  ambient_light_uniform_update(light);
}
