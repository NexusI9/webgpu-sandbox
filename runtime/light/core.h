#ifndef _LIGHT_CORE_H_
#define _LIGHT_CORE_H_

#include <cglm/cglm.h>
#include <stdint.h>
#include <cglm/types.h>

#include "backend/registry.h"
#include "backend/ssbo.h"
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
  reg_id_t id;
  vec3 position;
  vec3 color;
  float intensity;
  float cutoff;
  float inner_cutoff;
  float near;
  float far;
  SSBOSlot ssbo_slot[LIGHT_SSBO_SLOT_COUNT + 5]; // 1 list + (1 + 5 views)
  Projection views;
} PointLight;

typedef struct {
  reg_id_t id;
  vec3 position; // abstract, for UI purpose only
  vec3 color;
  float intensity;
  SSBOSlot ssbo_slot;
  Projection views;
} AmbientLight;

typedef struct {
  reg_id_t id;
  vec3 position;
  vec3 target;
  vec3 color;
  float cutoff;
  float angle;
  float inner_cutoff;
  float intensity;
  SSBOSlot ssbo_slot[LIGHT_SSBO_SLOT_COUNT];
  Projection views;
} SpotLight;

typedef struct {
  reg_id_t id;
  vec3 position;
  vec3 color;
  float size;
  float intensity;
  SSBOSlot ssbo_slot[LIGHT_SSBO_SLOT_COUNT];
  Projection views;
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
void light_point_create(PointLight *, PointLightDescriptor *);
void light_spot_create(SpotLight *, SpotLightDescriptor *);
void light_ambient_create(AmbientLight *, AmbientLightDescriptor *);
void light_sun_create(SunLight *, SunLightDescriptor *);

static inline void light_point_projection_update(PointLight *light) {
  // update light projection attribute
  projection_point(&light->views, light->position, light->near, light->far);

  // transfert attribute to SSBO slot
  projection_update_ssbo_slot(light->ssbo_slot, &light->views,
                                LightSSBOSlot_View);
}

static inline void light_spot_projection_update(SpotLight *light) {
  // update light projection attribute
  projection_spot(&light->views, light->position, light->target, light->angle);

  // transfert attribute to SSBO slot
  projection_update_ssbo_slot(light->ssbo_slot, &light->views,
                                LightSSBOSlot_View);
}

static inline void light_sun_projection_update(SunLight *light) {
  // update light projection attribute
  projection_sun(&light->views, light->position, light->size);

  // transfert attribute to SSBO slot
  projection_update_ssbo_slot(light->ssbo_slot, &light->views,
                                LightSSBOSlot_View);
}

#endif
