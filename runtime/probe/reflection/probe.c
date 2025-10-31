#include "probe.h"

#include <cglm/cam.h>
#include <cglm/vec3.h>
#include <stdint.h>

#include "backend/registry.h"
#include "backend/ubo.h"
#include "core.h"
#include "grid.h"
#include "runtime/camera/core.h"
#include "runtime/camera/uniform.h"
#include "utils/dyli.h"
#include "utils/projection.h"

/*

▗▄▄▖ ▗▄▄▖  ▗▄▖ ▗▄▄▖ ▗▄▄▄▖
▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌
▐▛▀▘ ▐▛▀▚▖▐▌ ▐▌▐▛▀▚▖▐▛▀▀▘
▐▌   ▐▌ ▐▌▝▚▄▞▘▐▙▄▞▘▐▙▄▄▖

 */

void probe_reflection_create(ProbeReflection *probe, vec3 position) {

  probe->id = reg_register(probe, RegEntryType_ProbeReflection);
  glm_vec3_copy(position, probe->position);

  for (uint8_t i = 0; i < PROBE_REFLECTION_VIEW_COUNT; i++) {

    // shallow camera
    Camera *cam = &probe->camera[i];
    camera_create(cam, &(CameraCreateDescriptor){0});

    glm_vec3_copy((float *)probe->position, cam->position);
    glm_vec3_copy((float *)projection_cubemaps_directions[i], cam->forward);
    glm_vec3_copy((float *)projection_cubemaps_ups[i], cam->up);
  }
}

void probe_reflection_update_uniform(ProbeReflection *probe) {

  ProbeReflectionUniform *uniform =
      (ProbeReflectionUniform *)probe->ubo_uniform.uniform.reflection_probe;

  glm_vec3_copy(probe->position, uniform->position);
  uniform->radius = probe->radius;
}

void probe_reflection_update_camera(ProbeReflection *probe) {
  // transfert attribute to UBO slot
  for (uint8_t i = 0; i < PROBE_REFLECTION_VIEW_COUNT; i++) {
    Camera *cam = &probe->camera[i];

    camera_set_position(cam, probe->position);

    glm_vec3_add(cam->position, cam->forward, cam->target);

    glm_lookat(cam->position, cam->target, cam->up, cam->view);
    camera_uniform_update(cam);

    CameraUniform *uniform = camera_uniform(cam);
    ubo_slot_set_uniform(&probe->ubo_camera[i], (void *)uniform,
                         sizeof(CameraUniform));
  }
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

void probe_reflection_destroy(ProbeReflection *probe) {
  // TODO
}
