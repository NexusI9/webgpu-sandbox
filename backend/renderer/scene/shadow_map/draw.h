#ifndef _SHADOW_MAP_DRAW_H_
#define _SHADOW_MAP_DRAW_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include <stddef.h>
#include <webgpu/webgpu.h>

typedef struct {
  WGPUDevice device;
  WGPUQueue *queue;
  MeshRefList *mesh_list;
  LightList* lights;
} ShadowMapDrawAllDescriptor;

typedef struct {
  const WGPUDevice *device;
  WGPUQueue *queue;
  const WGPUCommandEncoder *encoder;
  PointLightList *light_list;
  const size_t light_index;
  MeshRefList *mesh_list;
} ShadowMapDrawPointLightDescriptor;

typedef struct {
  const WGPUDevice *device;
  WGPUQueue *queue;
  const WGPUCommandEncoder *encoder;
  SunLightList *light_list;
  WGPUTexture color_map;
  WGPUTexture depth_map;
  const size_t light_index;
  const size_t layer_index;
  MeshRefList *mesh_list;
} ShadowMapDrawSunLightDescriptor;

typedef struct {
  const WGPUDevice *device;
  WGPUQueue *queue;
  const WGPUCommandEncoder *encoder;
  SpotLightList *light_list;
  const size_t light_index;
  MeshRefList *mesh_list;
} ShadowMapDrawSpotLightDescriptor;

typedef struct {
  const WGPUDevice *device;
  WGPUQueue *queue;
  const WGPUCommandEncoder *encoder;
  WGPUTexture color_map;
  WGPUTexture depth_map;
  LightViews *views;
  const size_t layer_index;
  MeshRefList *mesh_list;
} ShadowMapDrawDirLightDescriptor;

typedef struct {
  MeshRefList *mesh_list;
  WGPUTexture color_texture;
  WGPUTexture depth_texture;
  uint32_t layer;
  const WGPUDevice *device;
  WGPUQueue *queue;
  WGPUCommandEncoder encoder;
} ShadowMapDrawDescriptor;

void shadow_map_draw_all(const ShadowMapDrawAllDescriptor *);

void shadow_map_draw_point_light(const ShadowMapDrawPointLightDescriptor *);
void shadow_map_draw_sun_light(const ShadowMapDrawSunLightDescriptor *);
void shadow_map_draw_spot_light(const ShadowMapDrawSpotLightDescriptor *);

#endif
