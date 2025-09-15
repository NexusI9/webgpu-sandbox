#ifndef _SHADOW_MAP_DRAW_H_
#define _SHADOW_MAP_DRAW_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/scene/debug/debug.h"
#include "../utils/projection.h"
#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  MeshRefList *mesh_list;
  LightList *lights;
} ShadowMapDrawAllDescriptor;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const WGPUCommandEncoder command_encoder;
  PointLight *light;
  const size_t texture_layer;
  RenderPass *pass;
} ShadowMapDrawPointLightDescriptor;

typedef struct {
  SceneDebug *scene_debug;
  const uint16_t max_views;
} ShadowMapDebug;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const WGPUCommandEncoder command_encoder;
  SunLight *light;
  const size_t texture_layer;
  RenderPass *pass;
} ShadowMapDrawSunLightDescriptor;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const WGPUCommandEncoder command_encoder;
  SpotLight *light;
  const size_t texture_layer;
  RenderPass *pass;
} ShadowMapDrawSpotLightDescriptor;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const WGPUCommandEncoder command_encoder;
  Projection *views;
  const size_t texture_layer;
  const ssbo_id_t ssbo_offset;
  const Pipeline *pipeline;
  RenderPass *pass;
} ShadowMapDrawDirLightDescriptor;

typedef struct {
  RenderPass *pass;
  const uint32_t texture_layer;
  const ssbo_id_t ssbo_offset;
  const WGPUDevice device;
  const WGPUQueue queue;
  WGPUCommandEncoder command_encoder;
  const Pipeline *pipeline;
} ShadowMapDrawDescriptor;

void shadow_map_draw_all(const ShadowMapDrawAllDescriptor *,
                         const ShadowMapDebug *);

void shadow_map_draw_point_light(const ShadowMapDrawPointLightDescriptor *,
                                 const ShadowMapDebug *);
void shadow_map_draw_sun_light(const ShadowMapDrawSunLightDescriptor *,
                               const ShadowMapDebug *);
void shadow_map_draw_spot_light(const ShadowMapDrawSpotLightDescriptor *,
                                const ShadowMapDebug *);

#endif
