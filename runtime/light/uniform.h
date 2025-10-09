#ifndef _LIGHT_LIST_UNIFORM_H_
#define _LIGHT_LIST_UNIFORM_H_

#include <cglm/types.h>
#include <stdint.h>

#include "core.h"

// light type
// NOTE: use __attribute__ on list AS WELL AS entries (pointlights...) else
// wrong alignment in list entries (i.e. _padding takes color.r value)
typedef struct {
  vec3 position;
  float cutoff;
  vec3 color;
  float intensity;
  mat4 views[LIGHT_POINT_VIEWS];
  float inner_cutoff;
  float near;
  float far;
  float _padding[21];
} __attribute__((aligned(16))) PointLightUniform;

typedef struct {
  vec3 color;
  float intensity;
  float _pad[60];
} __attribute__((aligned(16))) AmbientLightUniform;

typedef struct {
  vec3 position;
  float cutoff;
  vec3 target;
  float inner_cutoff;
  vec3 color;
  float intensity;
  mat4 view;
  float _pad[36];
} __attribute__((aligned(16))) SpotLightUniform;

typedef struct {
  vec3 position;
  float intensity;
  vec3 color;
  float _padding;
  mat4 view;
  float _pad[40];
} __attribute__((aligned(16))) SunLightUniform;

// light uniforms
typedef struct {
  uint32_t length;
  PointLightUniform entries[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) PointLightListUniform;

typedef struct {
  uint32_t length;
  AmbientLightUniform entries[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) AmbientLightListUniform;

typedef struct {
  uint32_t length;
  SpotLightUniform entries[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) SpotLightListUniform;

typedef struct {
  uint32_t length;
  SunLightUniform entries[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) SunLightListUniform;

/*
  DEPRECATED
  Light comparator is use to map each point light entry and each uniform to
  compare them.

             LightUniform     LightComparator         Light
            --------------    --------+-------     -----------
            position     ====>  pos  ==  pos  <===  position
            cutoff       ====>  ctf  ==  ctf  <===  cutoff
            intensity    ====>  ity  ==  ity  <===  intensity
                              ---------------
                                   compare

  Light comparator aims to make it clearer on how uniform data and callback
  data are compared together to eventually trigger the uniform update.

  Light comparator also aims to use a common type to check all types of lights.

 */

/* Creator */

void light_point_uniform_update(PointLight *);
void light_ambient_uniform_update(AmbientLight *);
void light_spot_uniform_update(SpotLight *);
void light_sun_uniform_update(SunLight *);

// === Attributes Accessor & Mutator ===

/*


   Position


 */
static inline void sun_light_get_position(SunLight *light, vec3 dest) {
  glm_vec3_copy(light->position, dest);
}

static inline void sun_light_set_position(SunLight *light, vec3 value) {
  glm_vec3_copy(value, light->position);
  light_sun_uniform_update(light);
}

static inline void spot_light_get_position(SpotLight *light, vec3 dest) {
  glm_vec3_copy(light->position, dest);
}

static inline void spot_light_set_position(SpotLight *light, vec3 value) {
  glm_vec3_copy(value, light->position);
  light_spot_uniform_update(light);
}

static inline void point_light_get_position(PointLight *light, vec3 dest) {
  glm_vec3_copy(light->position, dest);
}

static inline void point_light_set_position(PointLight *light, vec3 value) {
  glm_vec3_copy(value, light->position);
  light_point_uniform_update(light);
}

/*


   Color


 */
static inline void sun_light_get_color(SunLight *light, color dest) {
  glm_vec4_copy(light->color, dest);
}

static inline void sun_light_set_color(SunLight *light, color value) {
  glm_vec4_copy(value, light->color);
  light_sun_uniform_update(light);
}

static inline void spot_light_get_color(SpotLight *light, color dest) {
  glm_vec4_copy(light->color, dest);
}

static inline void spot_light_set_color(SpotLight *light, color value) {
  glm_vec4_copy(value, light->color);
  light_spot_uniform_update(light);
}

static inline void ambient_light_get_color(AmbientLight *light, color dest) {
  glm_vec4_copy(light->color, dest);
}

static inline void ambient_light_set_color(AmbientLight *light, color value) {
  glm_vec4_copy(value, light->color);
  light_ambient_uniform_update(light);
}

static inline void point_light_get_color(PointLight *light, color dest) {
  glm_vec4_copy(light->color, dest);
}

static inline void point_light_set_color(PointLight *light, color value) {
  glm_vec4_copy(value, light->color);
  light_point_uniform_update(light);
}

/*


   Intensity


 */
static inline float sun_light_get_intensity(SunLight *light) {
  return light->intensity;
}

static inline void sun_light_set_intensity(SunLight *light, float value) {
  light->intensity = value;
  light_sun_uniform_update(light);
}

static inline float spot_light_get_intensity(SpotLight *light) {
  return light->intensity;
}

static inline void spot_light_set_intensity(SpotLight *light, float value) {
  light->intensity = value;
  light_spot_uniform_update(light);
}

static inline float ambient_light_get_intensity(AmbientLight *light) {
  return light->intensity;
}

static inline void ambient_light_set_intensity(AmbientLight *light,
                                               float value) {
  light->intensity = value;
  light_ambient_uniform_update(light);
}

static inline float point_light_get_intensity(PointLight *light) {
  return light->intensity;
}

static inline void point_light_set_intensity(PointLight *light, float value) {
  light->intensity = value;
  light_point_uniform_update(light);
}

/*


   Cutoff


 */
static inline float point_light_get_cutoff(PointLight *light) {
  return light->cutoff;
}

static inline void point_light_set_cutoff(PointLight *light, float value) {
  light->cutoff = value;
  light_point_uniform_update(light);
}

static inline float point_light_get_inner_cutoff(PointLight *light) {
  return light->inner_cutoff;
}

static inline void point_light_set_inner_cutoff(PointLight *light,
                                                float value) {
  light->inner_cutoff = value;
  light_point_uniform_update(light);
}

static inline float spot_light_get_cutoff(SpotLight *light) {
  return light->cutoff;
}

static inline void spot_light_set_cutoff(SpotLight *light, float value) {
  light->cutoff = value;
  light_spot_uniform_update(light);
}

static inline float spot_light_get_inner_cutoff(SpotLight *light) {
  return light->inner_cutoff;
}

static inline void spot_light_set_inner_cutoff(SpotLight *light, float value) {
  light->inner_cutoff = value;
  light_spot_uniform_update(light);
}

/*


   Near / Far


 */
static inline float point_light_get_near(PointLight *light) {
  return light->near;
}

static inline void point_light_set_near(PointLight *light, float value) {
  light->near = value;
  light_point_uniform_update(light);
}

static inline float point_light_get_far(PointLight *light) {
  return light->far;
}

static inline void point_light_set_far(PointLight *light, float value) {
  light->far = value;
  light_point_uniform_update(light);
}

/*


   Angle


 */

static inline float spot_light_get_angle(SpotLight *light) {
  return light->angle;
}

static inline void spot_light_set_angle(SpotLight *light, float value) {
  light->angle = value;
  light_spot_uniform_update(light);
}

/*


   Size


 */

static inline float sun_light_get_angle(SunLight *light) { return light->size; }

static inline void sun_light_set_near(SunLight *light, float value) {
  light->size = value;
  light_sun_uniform_update(light);
}

#endif
