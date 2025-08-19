#ifndef _PROBE_GRID_LIST_H_
#define _PROBE_GRID_LIST_H_

#include "grid.h"

#define PROBE_REFLECTION_LIST_CAPACITY 16

typedef struct {
  ProbeReflectionGrid *entries;
  size_t capacity;
  size_t length;
} ProbeReflectionList;

DynamicListStatus probe_reflection_list_create(ProbeReflectionList *,
                                               const size_t);

DynamicListStatus probe_reflection_list_insert(ProbeReflectionList *,
                                               ProbeReflectionGrid *);

ProbeReflectionGrid *probe_reflection_list_new_entry(ProbeReflectionList *);

DynamicListStatus probe_reflection_list_remove(ProbeReflectionList *,
                                               ProbeReflectionGrid *);

DynamicListStatus probe_reflection_list_destroy(ProbeReflectionList *,
                                                ProbeReflectionGrid *);

#endif
