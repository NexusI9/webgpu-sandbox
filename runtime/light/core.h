#ifndef _LIGHT_CORE_H_
#define _LIGHT_CORE_H_

#include <cglm/cglm.h>
#include <cglm/types.h>
#include <stdint.h>

#include "backend/registry.h"
#include "backend/ssbo.h"
#include "utils/name.h"
#include "utils/projection.h"
#include "webgpu/webgpu.h"

#define LIGHT_POINT_VIEWS 6
#define LIGHT_SPOT_VIEW 1
#define LIGHT_MAX_CAPACITY 16

typedef enum {
  LightType_Ambient = 1 << 0,
  LightType_Spot = 1 << 1,
  LightType_Sun = 1 << 2,
  LightType_Point = 1 << 3,
} LightType;

#define LIGHT_SSBO_SLOT_COUNT 2

typedef enum {
  LightSSBOSlot_List,
  LightSSBOSlot_View,
} LightSSBOSlot;

// core type
typedef struct {
  name_t name;
  reg_id_t id;
  vec3 position;
  color color;
  float intensity;
  float cutoff;
  float inner_cutoff;
  float near;
  float far;
  SSBOSlot ssbo_slot[LIGHT_SSBO_SLOT_COUNT + 5]; // 1 list + (1 + 5 views)
  Projection views;
} PointLight;

typedef struct {
  name_t name;
  reg_id_t id;
  vec3 position; // abstract, for UI purpose only
  color color;
  float intensity;
  SSBOSlot ssbo_slot;
  Projection views;
} AmbientLight;

typedef struct {
  name_t name;
  reg_id_t id;
  vec3 position;
  vec3 target;
  color color;
  float cutoff;
  float angle;
  float inner_cutoff;
  float intensity;
  SSBOSlot ssbo_slot[LIGHT_SSBO_SLOT_COUNT];
  Projection views;
} SpotLight;

typedef struct {
  name_t name;
  reg_id_t id;
  vec3 position;
  color color;
  float size;
  float intensity;
  SSBOSlot ssbo_slot[LIGHT_SSBO_SLOT_COUNT];
  Projection views;
} SunLight;

// descriptor type
typedef struct {
  const char *name;
  vec3 position;
  color color;
  float intensity;
  float cutoff;
  float inner_cutoff;
  float near;
  float far;
} PointLightDescriptor;

typedef struct {
  const char *name;
  vec3 position;
  color color;
  float intensity;
} AmbientLightDescriptor;

typedef struct {
  const char *name;
  vec3 position;
  vec3 target;
  color color;
  float cutoff;
  float angle;
  float inner_cutoff;
  float intensity;
} SpotLightDescriptor;

typedef struct {
  const char *name;
  vec3 position;
  color color;
  float size;
  float intensity;
} SunLightDescriptor;

typedef struct {
  WGPUTextureView texture;
  WGPUSampler sampler;
} LightTexture;

/* =============================== SHADOW PROCESS ==============================
  Shadows use a Shadow Map approach. Meaning that they render multiple
  time the scene but under various view angles (each lights angles) to generate
  Depth Maps.
   1.Point light use a Cube Shadow Map: meaning that we will use
   our point light as a cube rendering 6 times ou scene with different angles
   2. For Spot light use Cascade Shadow Map

   To Generate the Depth Map we only require a Vertex Shader (no Fragment) as to
  only traslate the vertices under the light projection point of view

  We will then store those Depth Maps in each lights as TextureView
  and Sampler
  Once our Depth Map are stored we can finally use them in our "base" shaders

  Process Diagram:

                +----------------------+
                |        Light         |
                +----------------------+
                          |
                   Light Projection
                    (cube/cascade)
                          |
                  *****************
                  * Render pass 1 *
                  *****************
                          |
                  Generate Depth Map
                          |
                    Store Depth Map
                       Texture
                          |
                +----------------------+
                |        Mesh          |
                +----------------------+
                          |
                  Bind Depth Texture
                          |
                    Compare with
                      Fragment
                          |
                  *****************
                  * Render pass 2 *
                  *****************

  ===========================================================================

 */

// constructors
void point_light_create(PointLight *, PointLightDescriptor *);
void spot_light_create(SpotLight *, SpotLightDescriptor *);
void ambient_light_create(AmbientLight *, AmbientLightDescriptor *);
void sun_light_create(SunLight *, SunLightDescriptor *);

// === Matrix Updates ===
static inline void point_light_projection_update(PointLight *light) {
  // update light projection attribute
  projection_point(&light->views, light->position, light->near, light->far);

  // transfert attribute to SSBO slot
  projection_update_ssbo_slot(light->ssbo_slot, &light->views,
                              LightSSBOSlot_View);
}

static inline void spot_light_projection_update(SpotLight *light) {
  // update light projection attribute
  projection_spot(&light->views, light->position, light->target, light->angle);

  // transfert attribute to SSBO slot
  projection_update_ssbo_slot(light->ssbo_slot, &light->views,
                              LightSSBOSlot_View);
}

static inline void sun_light_projection_update(SunLight *light) {
  // update light projection attribute
  projection_sun(&light->views, light->position, light->size);

  // transfert attribute to SSBO slot
  projection_update_ssbo_slot(light->ssbo_slot, &light->views,
                              LightSSBOSlot_View);
}

static inline const char *point_light_get_name(PointLight *light) {
  return light->name;
}

static inline void point_light_set_name(PointLight *light, const char *src) {
  name_copy(src, light->name);
}

static inline const char *spot_light_get_name(SpotLight *light) {
  return light->name;
}

static inline void spot_light_set_name(SpotLight *light, const char *src) {
  name_copy(src, light->name);
}

static inline const char *sun_light_get_name(SunLight *light) {
  return light->name;
}

static inline void sun_light_set_name(SunLight *light, const char *src) {
  name_copy(src, light->name);
}

static inline const char *ambient_light_get_name(AmbientLight *light) {
  return light->name;
}

static inline void ambient_light_set_name(AmbientLight *light,
                                          const char *src) {
  name_copy(src, light->name);
}

#endif
