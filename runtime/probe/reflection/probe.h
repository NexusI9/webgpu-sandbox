#ifndef _PROBE_REFLECTION_PROBE_H_
#define _PROBE_REFLECTION_PROBE_H_

#include <cglm/cglm.h>
#include <cglm/types.h>
#include <stddef.h>

#include "backend/registry.h"
#include "backend/ubo.h"
#include "core.h"
#include "runtime/camera/core.h"
#include "runtime/probe/uniform.h"
#include "utils/dyli.h"
#include "utils/projection.h"

// See probe reflection limitations:
// https://discussions.unity.com/t/reflection-probe-inaccuracy/675384/3
// tl;rd: Probe Reflections Cubes are more suitable for spherical and complex
// object where accuracy doesn't really matter.

#define PROBE_REFLECTION_VIEW_COUNT 6

typedef struct {
  reg_id_t id;
  vec3 position;
  float radius;
  Camera camera[PROBE_REFLECTION_VIEW_COUNT];
  ProbeListSlot ubo_uniform;
  UBOSlot ubo_camera[PROBE_REFLECTION_VIEW_COUNT];
} ProbeReflection;

typedef struct {
  ProbeReflection **entries;
  size_t length;
  size_t capacity;
} ProbeReflectionList;

/* === Probe Reflection === */

void probe_reflection_create(ProbeReflection *, vec3);
void probe_reflection_destroy(ProbeReflection *);

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
