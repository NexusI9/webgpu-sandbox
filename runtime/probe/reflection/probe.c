#include "probe.h"
#include "grid.h"

/*


▗▄▄▖ ▗▄▄▖  ▗▄▖ ▗▄▄▖ ▗▄▄▄▖    ▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌       ▐▌     █  ▐▌     █
▐▛▀▘ ▐▛▀▚▖▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘    ▐▌     █   ▝▀▚▖  █
▐▌   ▐▌ ▐▌▝▚▄▞▘▐▙▄▞▘▐▙▄▄▖    ▐▙▄▄▖▗▄█▄▖▗▄▄▞▘  █



 */

DynamicListStatus probe_reflection_list_create(ProbeReflectionList *list,
                                               const size_t capacity) {
  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(ProbeReflection), capacity,
                     "Probe Reflection list");
}

DynamicListStatus probe_reflection_list_insert(ProbeReflectionList *list,
                                               ProbeReflection *entry) {

  // temporary (shader require static length for now)
  if (list->length == PROBE_REFLECTION_LIST_MAX_COUNT)
    return DynamicListStatus_UndefError;

  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(ProbeReflection), (void *)entry, 1,
                     "Probe Reflection list");
}

ProbeReflection *probe_reflection_list_new_entry(ProbeReflectionList *list) {

  // temporary (shader require static length for now)
  if (list->length == PROBE_REFLECTION_LIST_MAX_COUNT)
    return NULL;

  return (ProbeReflection *)dyli_new_entry(
      (void *)&list->entries, &list->capacity, &list->length,
      sizeof(ProbeReflection), "Probe Reflection list");
}

DynamicListStatus probe_reflection_list_remove(ProbeReflectionList *list,
                                               ProbeReflection *entry) {
  return dyli_remove((void *)list->entries, &list->length,
                     sizeof(ProbeReflection), (void *)entry,
                     "Probe Reflection list");
}

DynamicListStatus probe_reflection_list_destroy(ProbeReflectionList *list) {
  return dyli_free((void *)list->entries, &list->capacity, &list->length);
}
