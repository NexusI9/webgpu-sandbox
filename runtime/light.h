#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "mesh.h"
#include "shader.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <stdint.h>

#define LIGHT_MAX_CAPACITY 16
#define LIGHT_POINT_VIEWS 6
#define LIGHT_SPOT_VIEW 1
#define LIGHT_SUN_DISTANCE 10

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
  vec3 position; // abstract, for UI purpose only
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
} SpotLight;

typedef struct {
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
  PointLight entries[LIGHT_MAX_CAPACITY];
  WGPUTextureView color_map;
  WGPUTextureView depth_map;
} PointLightList;

typedef struct {
  size_t length;
  size_t capacity;
  SpotLight entries[LIGHT_MAX_CAPACITY];
  WGPUTextureView color_map;
  WGPUTextureView depth_map;
} SpotLightList;

typedef struct {
  size_t length;
  size_t capacity;
  AmbientLight entries[LIGHT_MAX_CAPACITY];
} AmbientLightList;

typedef struct {
  size_t length;
  size_t capacity;
  SunLight entries[LIGHT_MAX_CAPACITY];
} SunLightList;

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  Camera *camera;
  Viewport *viewport;
  MeshList *list;
  MeshIndexedList *destination;
} LightCreateMeshDescriptor;

// constructors
void light_create_point(PointLight *, PointLightDescriptor *);
void light_create_spot(SpotLight *, SpotLightDescriptor *);
void light_create_ambient(AmbientLight *, AmbientLightDescriptor *);
void light_create_sun(SunLight *, SunLightDescriptor *);

// gizmo generation
void light_point_create_mesh(PointLight *, const LightCreateMeshDescriptor *);
void light_spot_create_mesh(SpotLight *, const LightCreateMeshDescriptor *);
void light_ambient_create_mesh(AmbientLight *,
                               const LightCreateMeshDescriptor *);
void light_sun_create_mesh(SunLight *, const LightCreateMeshDescriptor *);

// projections/view computing
LightViews light_point_views(vec3, float, float);
LightViews light_spot_view(vec3, vec3, float);
LightViews light_sun_view(vec3, float);
#endif
