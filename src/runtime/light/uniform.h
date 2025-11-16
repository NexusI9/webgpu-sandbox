#ifndef _LIGHT_LIST_UNIFORM_H_
#define _LIGHT_LIST_UNIFORM_H_

#include "core.h"
#include "runtime/light/list.h"
#include <cglm/types.h>
#include <stdint.h>

EXTERN_C_BEGIN

/* Creator */
#define _(Type, Label) void Label##_light_uniform_update(Type *);
LIGHT_TYPES(_)
#undef _

// === Attributes Accessor & Mutator ===

// clang-format off
//    Light Type | Light Label | Attribute Type | Attribute Label
#define LIGHTS_SCALAR_ATTRIBUTES(_) \
  _(  PointLight,      point,     float,      intensity       )   \
  _(  PointLight,      point,     float,      cutoff          )   \
  _(  PointLight,      point,     float,      inner_cutoff    )   \
  _(  PointLight,      point,     float,      near            )   \
  _(  PointLight,      point,     float,      far             )   \
                                                                  \
  _(  SpotLight,       spot,      float,      cutoff          )   \
  _(  SpotLight,       spot,      float,      inner_cutoff    )   \
  _(  SpotLight,       spot,      float,      intensity       )   \
  _(  SpotLight,       spot,      float,      angle           )   \
                                                                  \
  _(  SunLight,        sun,       float,      intensity       )   \
  _(  SunLight,        sun,       float,      size            )   \
                                                                  \
  _(  AmbientLight,    ambient,   float,      intensity       )

//   Light Type | Light Label | Attribute Type | GLM Alias | Attribute Label
#define LIGHTS_VECTOR_ATTRIBUTES(_) \
  _(  PointLight,      point,     vec3,       vec3,      position       )   \
  _(  PointLight,      point,     color,      vec4,      color          )   \
                                                                           \
  _(  SpotLight,       spot,      vec3,       vec3,      position       )   \
  _(  SpotLight,       spot,      vec3,       vec3,      target         )   \
  _(  SpotLight,       spot,      color,      vec4,      color          )   \
                                                                           \
  _(  SunLight,        sun,       vec3,       vec3,      position       )   \
  _(  SunLight,        sun,       color,      vec4,      color          )   \
                                                                           \
  _(  AmbientLight,    ambient,   color,      vec4,      color          )

// clang-format on

#define _(LightType, LightLabel, AttrType, AttrLabel)                          \
  static inline AttrType LightLabel##_light_get_##AttrLabel(                   \
      LightType *light) {                                                      \
    return light->AttrLabel;                                                   \
  }                                                                            \
                                                                               \
  static inline void LightLabel##_light_set_##AttrLabel(                       \
      LightType *light, const AttrType value) {                                \
    light->AttrLabel = value;                                                  \
    LightLabel##_light_uniform_update(light);                                  \
  }

LIGHTS_SCALAR_ATTRIBUTES(_);
#undef _

#define _(LightType, LightLabel, AttrType, GLMAlias, AttrLabel)                \
  static inline void LightLabel##_light_get_##AttrLabel(LightType *light,      \
                                                        AttrType dest) {       \
    glm_##GLMAlias##_copy(light->AttrLabel, dest);                             \
  }                                                                            \
                                                                               \
  static inline void LightLabel##_light_set_##AttrLabel(                       \
      LightType *light, const AttrType value) {                                \
    glm_##GLMAlias##_copy((float *)value, light->AttrLabel);                   \
    LightLabel##_light_uniform_update(light);                                  \
  }

LIGHTS_VECTOR_ATTRIBUTES(_);
#undef _

EXTERN_C_END

#endif
