#ifndef _LIGHT_LIST_H_
#define _LIGHT_LIST_H_
#include "../utils/stli.h"
#include "core.h"
#include "webgpu/webgpu.h"

typedef enum {
  LightListStatus_Success,
  LightListStatus_AlloFail,
  LightListStatus_Error,
} LightListStatus;

typedef enum {
  LightShadow_None = 0,
  LightShadow_Enabled = 1,
} LightShadow;

// light list
typedef struct {
  size_t length;
  size_t capacity;
  PointLight entries[LIGHT_MAX_CAPACITY];
} PointLightListBase;

typedef struct {
  size_t length;
  size_t capacity;
  PointLight *entries[LIGHT_MAX_CAPACITY];
  WGPUTexture color_map;
  WGPUTexture depth_map;
  WGPUTextureView color_view;
  WGPUTextureView depth_view;
} PointLightListShadow;

typedef struct {
  size_t length;
  size_t capacity;
  SpotLight entries[LIGHT_MAX_CAPACITY];
} SpotLightListBase;

typedef struct {
  size_t length;
  size_t capacity;
  SpotLight *entries[LIGHT_MAX_CAPACITY];
  WGPUTexture color_map;
  WGPUTexture depth_map;
  WGPUTextureView color_view;
  WGPUTextureView depth_view;
} SpotLightListShadow;

typedef struct {
  size_t length;
  size_t capacity;
  AmbientLight entries[LIGHT_MAX_CAPACITY];
} AmbientLightList;

typedef struct {
  size_t length;
  size_t capacity;
  SunLight entries[LIGHT_MAX_CAPACITY];
} SunLightListBase;

typedef struct {
  size_t length;
  size_t capacity;
  SunLight *entries[LIGHT_MAX_CAPACITY];
  // uses Spotlight list shadow depth and color map
} SunLightListShadow;

typedef struct {
  PointLightListBase base;
  PointLightListShadow shadow;
} PointLightList;

typedef struct {
  SunLightListBase base;
  SunLightListShadow shadow;
} SunLightList;

typedef struct {
  SpotLightListBase base;
  SpotLightListShadow shadow;
} SpotLightList;

typedef struct {
  PointLightList point;
  SunLightList sun;
  SpotLightList spot;
  AmbientLightList ambient;
} LightList;

StaticListStatus light_list_create(LightList *, size_t);

StaticListStatus light_list_point_shadow_insert(PointLightListShadow *,
                                                PointLight *);
StaticListStatus light_list_point_shadow_remove(PointLightListShadow *,
                                                PointLight *);

StaticListStatus light_list_sun_shadow_insert(SunLightListShadow *, SunLight *);
StaticListStatus light_list_sun_shadow_remove(SunLightListShadow *, SunLight *);

StaticListStatus light_list_spot_shadow_insert(SpotLightListShadow *,
                                               SpotLight *);
StaticListStatus light_list_spot_shadow_remove(SpotLightListShadow *,
                                               SpotLight *);

#endif
