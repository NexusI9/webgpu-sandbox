#ifndef _PROBE_GRID_H_
#define _PROBE_GRID_H_

#include "../backend/renderer/scene/scene.h"
#include "../utils/vector/vector.h"
#include "probe.h"
#include "webgpu/webgpu.h"

#define PROBE_REFLECTION_RESOLUTION 512
#define PROBE_REFLECTION_GRID_DIMENSION 3

#define PROBE_REFLECTION_GRID_MAX_COUNT 3
// N^3 max probes per grid
#define PROBE_REFLECTION_LIST_MAX_COUNT                                        \
  PROBE_REFLECTION_GRID_MAX_COUNT *PROBE_REFLECTION_GRID_MAX_COUNT             \
      *PROBE_REFLECTION_GRID_MAX_COUNT

// 16 grid per list
#define PROBE_REFLECTION_GRID_LIST_CAPACITY 8

typedef struct {
  ProbeReflectionList probes;
  WGPUTexture texture;
  WGPUTextureView view;
  ivec3 count;
  vec3 size;
} ProbeReflectionGrid;

typedef struct {
  ProbeReflectionGrid *entries;
  size_t capacity;
  size_t length;
  RenderPass pass;
} ProbeReflectionGridList;

typedef struct {
  ivec3 count;
  vec3 size;
  const WGPUDevice device;
  const WGPUQueue queue;
} ProbeReflectionGridDescriptor;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  const RenderPassDrawList *draw_list;
  const TextureResolution resolution;
  const size_t capacity;
  const PipelineMultisampleCount multisample;
} ProbeReflectionGridListDescriptor;

typedef struct {
  ssbo_id_t view_offset;
} ProbeReflectionGridListPreprocessorData;

typedef struct {
  SceneDebug *scene_debug;
  const uint8_t max_views;
} ProbeReflectionGridListDebug;

typedef struct {
  uint32_t length;
  ProbeReflectionUniform entries[PROBE_REFLECTION_GRID_LIST_CAPACITY *
                                 PROBE_REFLECTION_LIST_MAX_COUNT];
} __attribute__((aligned(16))) ProbeReflectionListUniform;

void probe_reflection_grid_create(ProbeReflectionGrid *,
                                  ProbeReflectionGridDescriptor *);

void probe_reflection_grid_destroy(ProbeReflectionGrid *);

/* === Probe Grid List  === */

DynamicListStatus
probe_reflection_grid_list_create(ProbeReflectionGridList *,
                                  const ProbeReflectionGridListDescriptor *);

DynamicListStatus probe_reflection_grid_list_insert(ProbeReflectionGridList *,
                                                    ProbeReflectionGrid *);

ProbeReflectionGrid *
probe_reflection_grid_list_new_entry(ProbeReflectionGridList *);

DynamicListStatus probe_reflection_grid_list_remove(ProbeReflectionGridList *,
                                                    ProbeReflectionGrid *);

DynamicListStatus probe_reflection_grid_list_destroy(ProbeReflectionGridList *);

void probe_reflection_grid_list_draw(ProbeReflectionGridList *,
                                     ProbeReflectionGridListDebug *);

void probe_reflection_grid_list_draw_preprocessor(const RenderPass *, Mesh *,
                                                  void *);

void probe_reflection_grid_list_uniform(ProbeReflectionListUniform *,
                                        ProbeReflectionGridList *);

size_t probe_reflection_grid_list_probe_count(ProbeReflectionGridList *);
#endif
