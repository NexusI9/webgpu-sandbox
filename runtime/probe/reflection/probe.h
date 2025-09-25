#ifndef _PROBE_REFLECTION_PROBE_H_
#define _PROBE_REFLECTION_PROBE_H_

#include <cglm/cglm.h>
#include <stddef.h>
#include <cglm/types.h>

#include "backend/ssbo.h"
#include "utils/dyli.h"
#include "utils/projection.h"
#include "core.h"
#include "runtime/camera/core.h"

// See probe reflection limitations:
// https://discussions.unity.com/t/reflection-probe-inaccuracy/675384/3
// tl;rd: Probe Reflections Cubes are more suitable for spherical and complex
// object where accuracy doesn't really matter.

#define PROBE_REFLECTION_VIEW_COUNT 6

typedef struct {
  vec3 position;
  float radius;
  // 1 list + (1 + 5 view) like point lights
  SSBOSlot ssbo_slot[PROBE_REFLECTION_SSBO_SLOT_COUNT + 5];
  Camera camera[PROBE_REFLECTION_VIEW_COUNT];
} ProbeReflection;

typedef struct {
  vec3 position;
  float radius;
  float _pad[60];
} __attribute__((aligned(16))) ProbeReflectionUniform;

typedef struct {
  ProbeReflection *entries;
  size_t length;
  size_t capacity;
} ProbeReflectionList;

/* === Probe Reflection === */

void probe_reflection_create(ProbeReflection *, vec3);

void probe_reflection_update_uniform(ProbeReflection *);
void probe_reflection_update_camera(ProbeReflection *);

/* === Probe Reflection List  === */

DynamicListStatus probe_reflection_list_create(ProbeReflectionList *,
                                               const size_t);

DynamicListStatus probe_reflection_list_insert(ProbeReflectionList *,
                                               ProbeReflection *);

ProbeReflection *probe_reflection_list_new_entry(ProbeReflectionList *);

DynamicListStatus probe_reflection_list_remove(ProbeReflectionList *,
                                               ProbeReflection *);

DynamicListStatus probe_reflection_list_destroy(ProbeReflectionList *);

#endif
