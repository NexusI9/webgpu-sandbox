#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "shader.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <stdint.h>

#define LIGHT_MAX_CAPACITY 16
#define LIGHT_POINT_VIEWS 6
#define LIGHT_DIRECTIONAL_VIEW 1

// core type
typedef struct {
  vec3 position;
  vec3 color;
  float intensity;
  float cutoff;
  float inner_cutoff;
  float near;
  float far;
} PointLight;

typedef struct {
  vec3 color;
  float intensity;
} AmbientLight;

typedef struct {
  vec3 position;
  vec3 target;
  vec3 color;
  float cutoff;
  float angle;
  float inner_cutoff;
  float intensity;
} DirectionalLight;

// descriptor type
typedef struct {
  vec3 position;
  vec3 color;
  float intensity;
  float cutoff;
  float inner_cutoff;
  float near;
  float far;
} PointLightDescriptor;

typedef struct {
  vec3 color;
  float intensity;
} AmbientLightDescriptor;

typedef struct {
  vec3 position;
  vec3 target;
  vec3 color;
  float cutoff;
  float angle;
  float inner_cutoff;
  float intensity;
} DirectionalLightDescriptor;

// light type
// NOTE: use __attribute__ on list AS WELL AS items (pointlights...) else wrong
// alignment in list items (i.e. _padding takes color.r value)
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
} LightViews;

typedef struct {
  WGPUTextureView texture;
  WGPUSampler sampler;
} LightTexture;

// light list
typedef struct {
  size_t length;
  size_t capacity;
  PointLight items[LIGHT_MAX_CAPACITY];
  WGPUTextureView color_map;
  WGPUTextureView depth_map;
} PointLightList;

typedef struct {
  size_t length;
  size_t capacity;
  DirectionalLight items[LIGHT_MAX_CAPACITY];
  WGPUTextureView color_map;
  WGPUTextureView depth_map;
} DirectionalLightList;

typedef struct {
  size_t length;
  size_t capacity;
  AmbientLight items[LIGHT_MAX_CAPACITY];
} AmbientLightList;

void light_create_point(PointLight *, PointLightDescriptor *);
void light_create_directional(DirectionalLight *, DirectionalLightDescriptor *);
void light_create_ambient(AmbientLight *, AmbientLightDescriptor *);

LightViews light_point_views(vec3, float, float);
LightViews light_directional_view(vec3, vec3, float);
#endif
