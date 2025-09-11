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
  mat4 views[LIGHT_POINT_VIEWS]; // TODO: move the views into projection SSBO
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

#endif
