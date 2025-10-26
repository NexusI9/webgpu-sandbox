#ifndef _PROBE_UNIFORM_H_
#define _PROBE_UNIFORM_H_

#include "backend/ubo.h"
#include "utils/mem.h"
#include <cglm/cglm.h>
#include <stdint.h>

typedef enum {
  ProbeType_ReflectionPlane = 0,
  ProbeType_ReflectionProbe = 1,
  // ProbeType_Irradiance
} ProbeType;

typedef struct {
  vec3 position;
  float radius;
} __attribute__((aligned(16))) ProbeReflectionUniform;

typedef struct {
  vec3 position;
  float near;
  vec3 normal;
  float far;
  vec3 scale;
  float distance;
  vec3 tangent;
  float signed_distance;
  vec3 bitangent;
  uint32_t texture_layer;
  mat4 view;
} __attribute__((aligned(16))) ProbeReflectionPlaneUniform;

#define PROBE_LIST_ENTRIES_CAPACITY 16
typedef struct {
  ProbeReflectionPlaneUniform reflection_plane[PROBE_LIST_ENTRIES_CAPACITY];
  ProbeReflectionUniform reflection_probe[PROBE_LIST_ENTRIES_CAPACITY];

  uint32_t reflection_probe_count;
  uint32_t reflection_plane_count;
  uint32_t irradiance;
  uint32_t _pad0;

  float _pad[STRUCT_PAD(
      16, sizeof(ProbeReflectionPlaneUniform) * PROBE_LIST_ENTRIES_CAPACITY +
               sizeof(ProbeReflectionUniform) * PROBE_LIST_ENTRIES_CAPACITY +
               sizeof(uint32_t) * 4)];

} __attribute__((aligned(16))) ProbeListUniform;

// Each child probes of the list own a slot linked to the ProbeList entry
typedef struct {
  union { // points to ProbeListUniform array entry
    ProbeReflectionUniform *reflection_probe;
    ProbeReflectionPlaneUniform *reflection_plane;
  } uniform;
  size_t id;
  size_t offset; // offset within the whole ProbeListUniform
} ProbeListSlot;

/**
   Return the offset (in bytes) of the probe slot within the probe list uniform.
   This allows to update the buffer only where the probe got updated instead of
   the whole Probe List UBO each change.
 */
static inline size_t probe_list_uniform_offset(const ubo_id_t id,
                                               const ProbeType type) {

  static const size_t probe_uniform_size[] = {
      sizeof(ProbeReflectionPlaneUniform),
      sizeof(ProbeReflectionUniform),
  };

  size_t offset = 0;
  for (uint8_t i = 0; i < type; i++)
    offset += probe_uniform_size[i] * PROBE_LIST_ENTRIES_CAPACITY;

  return offset + id * sizeof(probe_uniform_size[type]);
}

ProbeListSlot probe_list_uniform_new_entry(ProbeListUniform *, const ProbeType);

#endif
