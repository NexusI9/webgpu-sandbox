#ifndef _LIGHT_LIST_H_
#define _LIGHT_LIST_H_
#include <stddef.h>
#include <stdint.h>

#include "backend/ubo.h"
#include "core.h"
#include "runtime/pipeline/render.h"
#include "backend/renderer/render_pass/core.h"
#include "backend/renderer/render_pass/render_pass.h"
#include "utils/mem.h"
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
  const size_t view_offset;
} LightShadowData;

// light list
typedef struct {
  size_t count;
  size_t capacity;
  PointLight *entries[LIGHT_MAX_CAPACITY];
} PointLightListBase;

typedef struct {
  size_t count;
  size_t capacity;
  PointLight *entries[LIGHT_MAX_CAPACITY];
  RenderPass pass;
} PointLightListShadow;

typedef struct {
  size_t count;
  size_t capacity;
  SpotLight *entries[LIGHT_MAX_CAPACITY];
} SpotLightListBase;

typedef struct {
  size_t count;
  size_t capacity;
  SpotLight *entries[LIGHT_MAX_CAPACITY];
  RenderPass pass;
} SpotLightListShadow;

typedef struct {
  size_t count;
  size_t capacity;
  AmbientLight *entries[LIGHT_MAX_CAPACITY];
} AmbientLightList;

typedef struct {
  size_t count;
  size_t capacity;
  SunLight *entries[LIGHT_MAX_CAPACITY];
} SunLightListBase;

typedef struct {
  size_t count;
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
  UBOSlot ubo_slot;
} LightList;

#define LIGHT_LIST_ENTRIES_CAPACITY 16

typedef struct {
  AmbientLightUniform ambient_light[LIGHT_LIST_ENTRIES_CAPACITY];
  PointLightUniform point_light[LIGHT_LIST_ENTRIES_CAPACITY];
  SpotLightUniform spot_light[LIGHT_LIST_ENTRIES_CAPACITY];
  SunLightUniform sun_light[LIGHT_LIST_ENTRIES_CAPACITY];

  uint32_t ambient_count;
  uint32_t point_count;
  uint32_t spot_count;
  uint32_t sun_count;

  float _pad[STRUCT_PAD(
      16, sizeof(AmbientLightUniform) * LIGHT_LIST_ENTRIES_CAPACITY +
              sizeof(PointLightUniform) * LIGHT_LIST_ENTRIES_CAPACITY +
              sizeof(SpotLightUniform) * LIGHT_LIST_ENTRIES_CAPACITY +
              sizeof(SunLightUniform) * LIGHT_LIST_ENTRIES_CAPACITY +
              sizeof(uint32_t) * 4)];
} __attribute__((aligned(16))) LightListUniform;

StaticListStatus light_list_create(LightList *, size_t);

PointLight *light_list_new_point_light(PointLightListBase *);
AmbientLight *light_list_new_ambient_light(AmbientLightList *);
SpotLight *light_list_new_spot_light(SpotLightListBase *);
SunLight *light_list_new_sun_light(SunLightListBase *);

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
  return list->spot.shadow.count + index;
}

/**
   Return the offset (in bytes) of the light slot within the light list uniform.
   This allows to update the buffer only where the light got updated instead of
   the whole Light List UBO each change.
 */
static inline size_t light_list_uniform_offset(const ubo_id_t id,
                                               const LightType type) {

  static const size_t light_uniform_size[] = {
      sizeof(AmbientLightUniform),
      sizeof(PointLightUniform),
      sizeof(SpotLightUniform),
      sizeof(SunLightUniform),
  };

  size_t offset = 0;
  for (uint8_t i = 0; i < type; i++)
    offset += light_uniform_size[i] * LIGHT_LIST_ENTRIES_CAPACITY;

  return offset + id * sizeof(light_uniform_size[type]);
}

LightListSlot light_list_uniform_new_entry(LightListUniform *, const LightType);

void light_list_uniform_update(LightList *);

#endif
