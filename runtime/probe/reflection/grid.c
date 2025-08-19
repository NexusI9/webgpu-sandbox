#include "grid.h"
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
  vec3_list_create(&grid->position, count);

  for (size_t x = 0; x < desc->count[0]; x++) {
    float x_pos = probe_reflection_point(x, desc->count[0], desc->size[0]);

    for (size_t y = 0; y < desc->count[1]; y++) {
      float y_pos = probe_reflection_point(y, desc->count[1], desc->size[1]);

      for (size_t z = 0; z < desc->count[2]; z++) {
        float z_pos = probe_reflection_point(z, desc->count[2], desc->size[2]);

        vec3_list_insert(&grid->position, (vec3){x_pos, y_pos, z_pos});
      }
    }
  }

  printf("[%p] init vec3 length: %lu\n", &grid->position,
         grid->position.length);
}
