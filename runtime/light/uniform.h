#ifndef _LIGHT_LIST_UNIFORM_H_
#define _LIGHT_LIST_UNIFORM_H_

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
  float _padding;
} __attribute__((aligned(16))) PointLightUniform;

typedef struct {
  vec3 color;
  float intensity;
} __attribute__((aligned(16))) AmbientLightUniform;

typedef struct {
  vec3 position;
  float cutoff;
  vec3 target;
  float inner_cutoff;
  vec3 color;
  float intensity;
  mat4 view;
} __attribute__((aligned(16))) SpotLightUniform;

typedef struct {
  vec3 position;
  float intensity;
  vec3 color;
  float _padding;
  mat4 view;
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

typedef struct {
  vec3 position;
} LightComparator;

/* Creator */

void point_light_uniform(PointLightUniform *, PointLight *);
void ambient_light_uniform(AmbientLightUniform *, AmbientLight *);
void spot_light_uniform(SpotLightUniform *, SpotLight *);
void sun_light_uniform(SunLightUniform *, SunLight *);

/* Callbacks */

void point_light_list_update_callback(void *, void *);
bool point_light_list_trigger_callback(void *, const void *);

void ambient_light_list_update_callback(void *, void *);
bool ambient_light_list_trigger_callback(void *, const void *);

void spot_light_list_update_callback(void *, void *);
bool spot_light_list_trigger_callback(void *, const void *);

void sun_light_list_update_callback(void *, void *);
bool sun_light_list_trigger_callback(void *, const void *);

#endif
