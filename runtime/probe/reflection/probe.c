#include "probe.h"
#include "grid.h"

/*


▗▄▄▖ ▗▄▄▖  ▗▄▖ ▗▄▄▖ ▗▄▄▄▖
▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌
▐▛▀▘ ▐▛▀▚▖▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘
▐▌   ▐▌ ▐▌▝▚▄▞▘▐▙▄▞▘▐▙▄▄▖



 */

void probe_reflection_create(ProbeReflection *probe, vec3 position) {

  probe->near = PROBE_REFLECTION_NEAR;
  probe->far = PROBE_REFLECTION_FAR;
  glm_vec3_copy(position, probe->position);

  ssbo_slot_init_alloc(&probe->ssbo_slot[ProbeReflectionSSBOField_List],
                       sizeof(ProbeReflectionUniform));

  probe_reflection_update_uniform(probe);

  for (uint8_t i = 0; i < PROBE_REFLECTION_VIEW_COUNT; i++)
    ssbo_slot_init_alloc(&probe->ssbo_slot[ProbeReflectionSSBOField_View + i],
                         sizeof(ProjectionUniform));

  probe_reflection_update_view(probe);
}

void probe_reflection_update_uniform(ProbeReflection *probe) {

  ProbeReflectionUniform *uniform =
      (ProbeReflectionUniform *)probe->ssbo_slot[ProbeReflectionSSBOField_List]
          .uniform;

  glm_vec3_copy(probe->position, uniform->position);
  uniform->radius = probe->radius;
}

void probe_reflection_update_view(ProbeReflection *probe) {
  // update light projection attribute
  projection_point(&probe->views, probe->position, probe->near, probe->far);

  // transfert attribute to SSBO slot
  projection_update_ssbo_slot(probe->ssbo_slot, &probe->views,
                                ProbeReflectionSSBOField_View);
}

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
