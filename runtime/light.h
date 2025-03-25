#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "../include/cglm/vec3.h"
#include <stdint.h>

#define LIGHT_MAX_CAPACITY 16

// descriptor type
typedef struct {
  vec3 position;
  vec3 color;
  float intensity;
} PointLightDescriptor;

typedef struct {
  vec3 color;
  float intensity;
} AmbientLightDescriptor;

typedef struct {
  vec3 position;
  vec3 target;
  vec3 color;
  float intensity;
} DirectionalLightDescriptor;

// light type
// NOTE: use __attribute__ on list AS WELL AS items (pointlights...) else wrong
// alignment in list items (i.e. _padding takes color.r value)
typedef struct {
  vec3 position;
  float _padding;
  vec3 color;
  float intensity;
} __attribute__((aligned(16))) PointLight;

typedef struct {
  vec3 color;
  float intensity;
} __attribute__((aligned(16))) AmbientLight;

typedef struct {
  vec3 position;
  float _padding_1;
  vec3 target;
  float _padding_2;
  vec3 color;
  float intensity;
} __attribute__((aligned(16))) DirectionalLight;

// light uniforms
typedef struct {
  uint32_t length;
  PointLight items[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) PointLightListUniform;

typedef struct {
  uint32_t length;
  AmbientLight items[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) AmbientLightListUniform;

typedef struct {
  uint32_t length;
  DirectionalLight items[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) DirectionalLightListUniform;

// light list
typedef struct {
  size_t length;
  PointLight items[LIGHT_MAX_CAPACITY];
  size_t capacity;
} PointLightList;

typedef struct {
  size_t length;
  DirectionalLight items[LIGHT_MAX_CAPACITY];
  size_t capacity;
} DirectionalLightList;

typedef struct {
  size_t length;
  AmbientLight items[LIGHT_MAX_CAPACITY];
  size_t capacity;
} AmbientLightList;

void light_create_point(PointLight *, PointLightDescriptor *);
void light_create_directional(DirectionalLight *, DirectionalLightDescriptor *);
void light_create_ambient(AmbientLight *, AmbientLightDescriptor *);

#endif
