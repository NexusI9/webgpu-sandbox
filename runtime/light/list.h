#ifndef _LIGHT_LIST_H_
#define _LIGHT_LIST_H_
#include <stddef.h>
#include <stdint.h>

#include "core.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "runtime/scene/renderer/render_pass/render_pass.h"
#include "utils/stli.h"
#include "webgpu/webgpu.h"

typedef enum {
  LightListStatus_Success,
  LightListStatus_AlloFail,
  LightListStatus_Error,
} LightListStatus;

typedef enum {
  LightCreateFlag_None = 0,
  LightCreateFlag_Shadow = 1 << 0,
} LightCreateFlag;

typedef struct {
  const RenderPipeline *pipeline;
  const size_t view_offset;
} LightShadowData;

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
  RenderPass pass;
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
  RenderPass pass;
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

typedef struct {
  uint32_t point;
  uint32_t spot;
  uint32_t sun;
  uint32_t ambient;
} LightCountUniform;

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

static inline size_t light_list_sun_layer_index(LightList *list, size_t index) {
  return list->spot.shadow.length + index;
}

#endif
