#ifndef _PROBE_REFLECTION_PROBE_H_
#define _PROBE_REFLECTION_PROBE_H_

#include "../utils/dyli.h"
#include <cglm/cglm.h>
#include <stddef.h>

#define PROBE_REFLECTION_VIEW_COUNT 6

typedef struct {
  vec3 position;
  mat4 view[PROBE_REFLECTION_VIEW_COUNT];
} ProbeReflection;

typedef struct {
  ProbeReflection *entries;
  size_t length;
  size_t capacity;
} ProbeReflectionList;

/* === Probe List  === */

DynamicListStatus probe_reflection_list_create(ProbeReflectionList *,
                                               const size_t);

DynamicListStatus probe_reflection_list_insert(ProbeReflectionList *,
                                               ProbeReflection *);

ProbeReflection *probe_reflection_list_new_entry(ProbeReflectionList *);

DynamicListStatus probe_reflection_list_remove(ProbeReflectionList *,
                                               ProbeReflection *);

DynamicListStatus probe_reflection_list_destroy(ProbeReflectionList *);

#endif
