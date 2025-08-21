#ifndef _PROBE_GRID_H_
#define _PROBE_GRID_H_

#include "../backend/renderer/scene/scene.h"
#include "../utils/vector/vector.h"
#include "probe.h"
#include "webgpu/webgpu.h"

#define PROBE_REFLECTION_GRID_COUNT 3
#define PROBE_REFLECTION_GRID_DIMENSION 3
#define PROBE_REFLECTION_LIST_CAPACITY 16

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
} ProbeReflectionGridList;

typedef struct {
  ivec3 count;
  vec3 size;
  const WGPUDevice device;
  const WGPUQueue queue;
} ProbeReflectionGridDescriptor;

void probe_reflection_grid_create(ProbeReflectionGrid *,
                                  ProbeReflectionGridDescriptor *);

void probe_reflection_grid_destroy(ProbeReflectionGrid *);

void probe_reflection_grid_draw(ProbeReflectionGrid *, SceneRenderer *);

/* === Probe Grid List  === */

DynamicListStatus probe_reflection_grid_list_create(ProbeReflectionGridList *,
                                                    const size_t);

DynamicListStatus probe_reflection_grid_list_insert(ProbeReflectionGridList *,
                                                    ProbeReflectionGrid *);

ProbeReflectionGrid *
probe_reflection_grid_list_new_entry(ProbeReflectionGridList *);

DynamicListStatus probe_reflection_grid_list_remove(ProbeReflectionGridList *,
                                                    ProbeReflectionGrid *);

DynamicListStatus probe_reflection_grid_list_destroy(ProbeReflectionGridList *);

#endif
