#ifndef _LIGHT_CORE_H_
#define _LIGHT_CORE_H_

#include <cglm/cglm.h>
#include <cglm/types.h>
#include <stdint.h>

#include "backend/registry.h"
#include "backend/ubo.h"
#include "utils/name.h"
#include "utils/projection.h"
#include "webgpu/webgpu.h"

#define LIGHT_POINT_VIEWS 6
#define LIGHT_SPOT_VIEW 1
#define LIGHT_MAX_CAPACITY 16

// explicitely define them cause order is important
#define LIGHT_TYPE_COUNT 4
typedef enum {
  LightType_Ambient = 0,
  LightType_Point = 1,
  LightType_Spot = 2,
  LightType_Sun = 3,
} LightType;

// light type
// NOTE: use __attribute__ on list AS WELL AS entries (pointlights...) else
// wrong alignment in list entries (i.e. _padding takes color.r value)
typedef struct {
  vec3 position;
  float cutoff;
  color color;
  mat4 views[LIGHT_POINT_VIEWS];
  float intensity;
  float inner_cutoff;
  float near;
  float far;
} __attribute__((aligned(16))) PointLightUniform;

typedef struct {
  color color;
  float intensity;
  float _pad[3];
} __attribute__((aligned(16))) AmbientLightUniform;

typedef struct {
  vec3 position;
  float cutoff;
  vec3 target;
  float inner_cutoff;
  color color;
  mat4 view;
  float intensity;
  float _pad[3];
} __attribute__((aligned(16))) SpotLightUniform;

typedef struct {
  vec3 position;
  float intensity;
  color color;
  mat4 view;
} __attribute__((aligned(16))) SunLightUniform;

typedef struct {
  union { // points to the LightListUniform entry
    AmbientLightUniform *ambient;
    PointLightUniform *point;
    SpotLightUniform *spot;
    SunLightUniform *sun;
  } uniform;
  size_t id;
  size_t offset;
} LightListSlot;

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
  Projection views;
  UBOSlot ubo_projection[LIGHT_POINT_VIEWS];
  LightListSlot ubo_uniform;
} PointLight;

typedef struct {
  name_t name;
  reg_id_t id;
  vec3 position; // abstract, for UI purpose only
  color color;
  float intensity;
  LightListSlot ubo_uniform;
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
  Projection views;
  UBOSlot ubo_projection;
  LightListSlot ubo_uniform;
} SpotLight;

typedef struct {
  name_t name;
  reg_id_t id;
  vec3 position;
  color color;
  float size;
  float intensity;
  Projection views;
  UBOSlot ubo_projection;
  LightListSlot ubo_uniform;
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

  // transfert attribute to UBO slot
  projection_update_ubo_slot(light->ubo_projection, &light->views);
}

static inline void spot_light_projection_update(SpotLight *light) {
  // update light projection attribute
  projection_spot(&light->views, light->position, light->target, light->angle);

  // transfert attribute to UBO slot
  projection_update_ubo_slot(&light->ubo_projection, &light->views);
}

static inline void sun_light_projection_update(SunLight *light) {
  // update light projection attribute
  projection_sun(&light->views, light->position, light->size);

  // transfert attribute to UBO slot
  projection_update_ubo_slot(&light->ubo_projection, &light->views);
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
