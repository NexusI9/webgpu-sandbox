#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <cglm/cglm.h>
#include "shader.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

#define LIGHT_MAX_CAPACITY 16
#define LIGHT_POINT_VIEWS 6
#define SHADOW_MAP_SIZE 512

// core type
typedef struct {
  vec3 position;
  vec3 color;
  float intensity;
} PointLight;

typedef struct {
  vec3 color;
  float intensity;
} AmbientLight;

typedef struct {

  vec3 position;
  vec3 target;
  vec3 color;
  float intensity;

} DirectionalLight;

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
  mat4 views[LIGHT_POINT_VIEWS];
} __attribute__((aligned(16))) PointLightUniform;

typedef struct {
  vec3 color;
  float intensity;
} __attribute__((aligned(16))) AmbientLightUniform;

typedef struct {
  vec3 position;
  float _padding_1;
  vec3 target;
  float _padding_2;
  vec3 color;
  float intensity;
} __attribute__((aligned(16))) DirectionalLightUniform;

// light uniforms
typedef struct {
  uint32_t length;
  PointLightUniform items[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) PointLightListUniform;

typedef struct {
  uint32_t length;
  AmbientLightUniform items[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) AmbientLightListUniform;

typedef struct {
  uint32_t length;
  DirectionalLightUniform items[LIGHT_MAX_CAPACITY];
} __attribute__((aligned(16))) DirectionalLightListUniform;

typedef struct {
  mat4 views[LIGHT_POINT_VIEWS];
  uint8_t length;
} PointLightViews;

// light list
typedef struct {
  size_t length;
  size_t capacity;
  PointLight items[LIGHT_MAX_CAPACITY];
  WGPUTextureView shadow_texture;
  WGPUSampler shadow_sampler;
} PointLightList;

typedef struct {
  size_t length;
  size_t capacity;
  DirectionalLight items[LIGHT_MAX_CAPACITY];
  WGPUTextureView shadow_texture;
  WGPUSampler shadow_sampler;
} DirectionalLightList;

typedef struct {
  size_t length;
  size_t capacity;
  AmbientLight items[LIGHT_MAX_CAPACITY];
} AmbientLightList;

void light_create_point(PointLight *, PointLightDescriptor *);
void light_create_directional(DirectionalLight *, DirectionalLightDescriptor *);
void light_create_ambient(AmbientLight *, AmbientLightDescriptor *);

PointLightViews light_point_views(vec3, viewport *);
#endif
