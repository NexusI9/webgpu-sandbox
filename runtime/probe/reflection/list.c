#include "list.h"
#include "grid.h"

DynamicListStatus probe_reflection_list_create(ProbeReflectionList *list,
                                               const size_t capacity) {
  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(ProbeReflectionGrid), capacity,
                     "Probe Reflection list");
}

DynamicListStatus probe_reflection_list_insert(ProbeReflectionList *list,
                                               ProbeReflectionGrid *entry) {
  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(ProbeReflectionGrid), (void *)entry, 1,
                     "Probe Reflection list");
}

ProbeReflectionGrid *
probe_reflection_list_new_entry(ProbeReflectionList *list) {

  return (ProbeReflectionGrid *)dyli_new_entry(
      (void *)&list->entries, &list->capacity, &list->length,
      sizeof(ProbeReflectionGrid), "Probe Reflection list");
}

DynamicListStatus probe_reflection_list_remove(ProbeReflectionList *list,
                                               ProbeReflectionGrid *entry) {
  return dyli_remove((void *)list->entries, &list->length,
                     sizeof(ProbeReflectionGrid), (void *)entry,
                     "Probe Reflection list");
}

DynamicListStatus probe_reflection_list_destroy(ProbeReflectionList *list,
                                                ProbeReflectionGrid *entry) {
  return dyli_free((void *)list->entries, &list->capacity, &list->capacity);
}
