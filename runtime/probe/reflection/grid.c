#include "grid.h"
#include "probe.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

static inline float probe_reflection_point(size_t x, uint16_t count,
                                           float size);

float probe_reflection_point(size_t x, uint16_t count, float size) {
  return count > 1 ? ((float)x * 2.0f * size / (count - 1)) - size : 0.0f;
}

void probe_reflection_grid_create(ProbeReflectionGrid *grid,
                                  ProbeReflectionGridDescriptor *desc) {

  glm_ivec3_copy(desc->count, grid->count);
  glm_vec3_copy(desc->size, grid->size);

  size_t count = desc->count[0] * desc->count[1] * desc->count[2];
  probe_reflection_list_create(&grid->probes, count);

  for (size_t x = 0; x < desc->count[0]; x++) {
    float x_pos = probe_reflection_point(x, desc->count[0], desc->size[0]);

    for (size_t y = 0; y < desc->count[1]; y++) {
      float y_pos = probe_reflection_point(y, desc->count[1], desc->size[1]);

      for (size_t z = 0; z < desc->count[2]; z++) {
        float z_pos = probe_reflection_point(z, desc->count[2], desc->size[2]);

        ProbeReflection *probe = probe_reflection_list_new_entry(&grid->probes);
        glm_vec3_copy((vec3){x_pos, y_pos, z_pos}, probe->position);
      }
    }
  }
}

void probe_reflection_grid_destroy(ProbeReflectionGrid *grid) {

  wgpuTextureViewRelease(grid->view);
  grid->view = NULL;

  wgpuTextureRelease(grid->texture);
  grid->texture = NULL;

  glm_ivec3_zero(grid->count);
  glm_vec3_zero(grid->size);

  probe_reflection_list_destroy(&grid->probes);
};

/*


 ▗▄▄▖▗▄▄▖ ▗▄▄▄▖▗▄▄▄     ▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
▐▌   ▐▌ ▐▌  █  ▐▌  █    ▐▌     █  ▐▌     █
▐▌▝▜▌▐▛▀▚▖  █  ▐▌  █    ▐▌     █   ▝▀▚▖  █
▝▚▄▞▘▐▌ ▐▌▗▄█▄▖▐▙▄▄▀    ▐▙▄▄▖▗▄█▄▖▗▄▄▞▘  █


 */

DynamicListStatus
probe_reflection_grid_list_create(ProbeReflectionGridList *list,
                                  const size_t capacity) {
  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(ProbeReflectionGrid), capacity,
                     "Probe Reflection Grid list");
}

DynamicListStatus
probe_reflection_grid_list_insert(ProbeReflectionGridList *list,
                                  ProbeReflectionGrid *entry) {
  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(ProbeReflectionGrid), (void *)entry, 1,
                     "Probe Reflection Grid list");
}

ProbeReflectionGrid *
probe_reflection_grid_list_new_entry(ProbeReflectionGridList *list) {

  return (ProbeReflectionGrid *)dyli_new_entry(
      (void *)&list->entries, &list->capacity, &list->length,
      sizeof(ProbeReflectionGrid), "Probe Reflection Grid list");
}

DynamicListStatus
probe_reflection_grid_list_remove(ProbeReflectionGridList *list,
                                  ProbeReflectionGrid *entry) {
  return dyli_remove((void *)list->entries, &list->length,
                     sizeof(ProbeReflectionGrid), (void *)entry,
                     "Probe Reflection Grid list");
}

DynamicListStatus
probe_reflection_grid_list_destroy(ProbeReflectionGridList *list) {
  return dyli_free((void *)list->entries, &list->capacity, &list->length);
}
