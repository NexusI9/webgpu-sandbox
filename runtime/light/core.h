#ifndef _LIGHT_CORE_H_
#define _LIGHT_CORE_H_

#include "../backend/registry.h"
#include "../mesh/mesh.h"
#include "../viewport/viewport.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <stdint.h>

#define LIGHT_POINT_VIEWS 6
#define LIGHT_SPOT_VIEW 1
#define LIGHT_MAX_CAPACITY 16
#define LIGHT_SUN_DISTANCE 10

typedef enum {
  LightType_Ambient = 1 << 0,
  LightType_Spot = 1 << 1,
  LightType_Sun = 1 << 2,
  LightType_Point = 1 << 3,
} LightType;

// core type
typedef struct {
  id_t id;
  vec3 position;
  vec3 color;
  float intensity;
  float cutoff;
  float inner_cutoff;
  float near;
  float far;
} PointLight;

typedef struct {
  id_t id;
  vec3 position; // abstract, for UI purpose only
  vec3 color;
  float intensity;
} AmbientLight;

typedef struct {
  id_t id;
  vec3 position;
  vec3 target;
  vec3 color;
  float cutoff;
  float angle;
  float inner_cutoff;
  float intensity;
} SpotLight;

typedef struct {
  id_t id;
  vec3 position;
  vec3 color;
  float size;
  float intensity;
} SunLight;

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
  vec3 position;
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
} SpotLightDescriptor;

typedef struct {
  vec3 position;
  vec3 color;
  float size;
  float intensity;
} SunLightDescriptor;

typedef struct {
  WGPUTextureView texture;
  WGPUSampler sampler;
} LightTexture;

#endif
