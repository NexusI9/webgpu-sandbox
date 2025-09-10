#ifndef _PROBE_GRID_H_
#define _PROBE_GRID_H_

#include "./core.h"
#include "probe.h"

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
  vec3 scale;
  vec3 position;
  AABB boundbox;
} ProbeReflectionGrid;

typedef struct {
  ProbeReflectionGrid *entries;
  size_t capacity;
  size_t length;
  RenderPass pass;
} ProbeReflectionGridList;

typedef struct {
  ivec3 count;
  vec3 scale;
  vec3 position;
  const WGPUDevice device;
  const WGPUQueue queue;
} ProbeReflectionGridDescriptor;

typedef struct {
  uint32_t length;
  ProbeReflectionUniform entries[PROBE_REFLECTION_GRID_LIST_CAPACITY *
                                 PROBE_REFLECTION_LIST_MAX_COUNT];
} __attribute__((aligned(16))) ProbeReflectionListUniform;

void probe_reflection_grid_create(ProbeReflectionGrid *,
                                  ProbeReflectionGridDescriptor *);

void probe_reflection_grid_destroy(ProbeReflectionGrid *);

void probe_reflection_grid_update_boundbox(ProbeReflectionGrid *);

/* === Probe Grid List  === */

DynamicListStatus
probe_reflection_grid_list_create(ProbeReflectionGridList *,
                                  const ProbeReflectionListDescriptor *);

DynamicListStatus probe_reflection_grid_list_insert(ProbeReflectionGridList *,
                                                    ProbeReflectionGrid *);

ProbeReflectionGrid *
probe_reflection_grid_list_new_entry(ProbeReflectionGridList *);

DynamicListStatus probe_reflection_grid_list_remove(ProbeReflectionGridList *,
                                                    ProbeReflectionGrid *);

DynamicListStatus probe_reflection_grid_list_destroy(ProbeReflectionGridList *);

void probe_reflection_grid_list_draw(ProbeReflectionGridList *,
                                     ProbeReflectionListDebug *);

void probe_reflection_grid_list_uniform(ProbeReflectionListUniform *,
                                        ProbeReflectionGridList *);

size_t probe_reflection_grid_list_probe_count(ProbeReflectionGridList *);
#endif
