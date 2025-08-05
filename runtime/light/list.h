#ifndef _LIGHT_LIST_H_
#define _LIGHT_LIST_H_
#include "core.h"


typedef enum{
  LightListStatus_Success,
  LightListStatus_AlloFail,
  LightListStatus_Error,
} LightListStatus;


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
  PointLightList point;
  SpotLightList spot;
  AmbientLightList ambient;
  SunLightList sun;
} LightList;


LightListStatus light_list_create(LightList*, size_t);


#endif
