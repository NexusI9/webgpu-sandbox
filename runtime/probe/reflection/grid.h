#ifndef _PROBE_GRID_H_
#define _PROBE_GRID_H_

#include "../utils/vector/vector.h"
#include "webgpu/webgpu.h"

#define PROBE_REFLECTION_GRID_COUNT 3
#define PROBE_REFLECTION_GRID_DIMENSION 3

typedef struct {
  Vec3List position;
  WGPUTexture texture;
  WGPUTextureView view;
  ivec3 count;
  vec3 size;
} ProbeReflectionGrid;

typedef struct {
  ivec3 count;
  vec3 size;
} ProbeReflectionGridDescriptor;

void probe_reflection_grid_create(ProbeReflectionGrid *,
                                  ProbeReflectionGridDescriptor *);

#endif
