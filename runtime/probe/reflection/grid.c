#include "grid.h"
#include "../backend/std_texture/std_texture.h"
#include "core.h"
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

  glm_vec3_copy(desc->scale, grid->scale);
  glm_vec3_copy(desc->position, grid->position);

  int clamp_count[PROBE_REFLECTION_GRID_DIMENSION]; // prevent overflow

  for (size_t i = 0; i < PROBE_REFLECTION_GRID_DIMENSION; i++)
    clamp_count[i] = glm_min(desc->count[i], PROBE_REFLECTION_GRID_MAX_COUNT);

  glm_ivec3_copy(clamp_count, grid->count);

  size_t count = clamp_count[0] * clamp_count[1] * clamp_count[2];
  probe_reflection_list_create(&grid->probes, count);

  for (size_t x = 0; x < clamp_count[0]; x++) {
    float x_pos = probe_reflection_point(x, clamp_count[0], desc->scale[0]);

    for (size_t y = 0; y < clamp_count[1]; y++) {
      float y_pos = probe_reflection_point(y, clamp_count[1], desc->scale[1]);

      for (size_t z = 0; z < clamp_count[2]; z++) {
        float z_pos = probe_reflection_point(z, clamp_count[2], desc->scale[2]);

        ProbeReflection *probe = probe_reflection_list_new_entry(&grid->probes);

        if (probe)
          probe_reflection_create(probe, (vec3){x_pos, y_pos, z_pos});
      }
    }
  }

  
  grid->view = std_texture_view(TextureViewType_FloatCubeArray);
  probe_reflection_grid_update_boundbox(grid);
}

void probe_reflection_grid_destroy(ProbeReflectionGrid *grid) {

  wgpuTextureViewRelease(grid->view);
  grid->view = NULL;

  wgpuTextureRelease(grid->texture);
  grid->texture = NULL;

  glm_ivec3_zero(grid->count);
  glm_vec3_zero(grid->scale);

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
                                  const ProbeReflectionListDescriptor *desc) {

  return probe_reflection_list_create_core(&(ProbeReflectionCreateCore){
      .device = desc->device,
      .queue = desc->queue,
      .probe_list =
          &(ProbeReflectionCreateCoreList){
              .entries = (void *)&list->entries,
              .capacity = &list->capacity,
              .length = &list->length,
              .type_size = sizeof(ProbeReflectionGrid),
              .label = "Probe Reflection Grid list",
              .num = desc->capacity,
          },
      .render_pass =
          &(ProbeReflectionCreateCorePass){
              .draw_list = desc->draw_list,
              .handle = &list->pass,
              .view_dimension = WGPUTextureViewDimension_CubeArray,
              .resolution = desc->resolution,
              .multisample = desc->multisample,
              .layer_count = PROBE_REFLECTION_GRID_LIST_CAPACITY *
                             PROBE_REFLECTION_LIST_MAX_COUNT,
          },
  });
}

DynamicListStatus
probe_reflection_grid_list_insert(ProbeReflectionGridList *list,
                                  ProbeReflectionGrid *entry) {

  // temporary (shader only accept static array for now)
  if (list->length == PROBE_REFLECTION_GRID_LIST_CAPACITY)
    return DynamicListStatus_UndefError;

  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(ProbeReflectionGrid), (void *)entry, 1,
                     "Probe Reflection Grid list");
}

ProbeReflectionGrid *
probe_reflection_grid_list_new_entry(ProbeReflectionGridList *list) {

  // temporary (shader only accept static array for now)
  if (list->length == PROBE_REFLECTION_GRID_LIST_CAPACITY)
    return NULL;

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

void probe_reflection_grid_list_draw(ProbeReflectionGridList *list,
                                     ProbeReflectionListDebug *debug) {

  // then update probe list texture cube array based on each probes views
  size_t layer = 0;

  render_pass_command_begin(&list->pass);
  {
    for (size_t i = 0; i < list->length; i++) {

      ProbeReflectionGrid *grid = &list->entries[i];

      TIMER("", {
        VERBOSE_PROCESS("Rendering Probe Reflection Grid %lu/%lu", i + 1,
                        list->length);

        for (size_t j = 0; j < grid->probes.length; j++) {

          ProbeReflection *probe = &grid->probes.entries[j];

          for (uint8_t k = 0; k < PROBE_REFLECTION_VIEW_COUNT; k++) {

            // define target layer
            WGPUTextureView target_color = wgpuTextureCreateView(
                list->pass.color.texture,
                &(WGPUTextureViewDescriptor){
                    .label = "Probe Reflection Target Color View",
                    .arrayLayerCount = 1,
                    .baseArrayLayer = layer,
                    .dimension = WGPUTextureViewDimension_2D,
                    .baseMipLevel = 0,
                    .mipLevelCount = 1,
                });

            WGPUTextureView target_depth = wgpuTextureCreateView(
                list->pass.depth.texture,
                &(WGPUTextureViewDescriptor){
                    .label = "Probe Reflection Target Depth View",
                    .arrayLayerCount = 1,
                    .baseArrayLayer = layer,
                    .dimension = WGPUTextureViewDimension_2D,
                    .baseMipLevel = 0,
                    .mipLevelCount = 1,
                });

            // update each mesh views/projections matrix
            render_pass_update_all_preprocessor_data(
                &list->pass,
                &(ProbeReflectionListPreprocessorData){
                    .view_offset =
                        probe->ssbo_slot[ProbeReflectionSSBOField_View + k].id,
                });

            // draw pass
            render_pass_command_draw(&list->pass, &(RenderPassDrawOptions){
                                                      .color = target_color,
                                                      .depth = target_depth,
                                                  });

            if (debug && layer < debug->max_views)
              scene_debug_view_create(debug->scene_debug, target_color);
            else
              wgpuTextureViewRelease(target_color);

            wgpuTextureViewRelease(target_depth);
            layer++;
          }
        }
      });
    }
  }
  render_pass_command_end(&list->pass);
}

void probe_reflection_grid_list_uniform(ProbeReflectionListUniform *uniform,
                                        ProbeReflectionGridList *grid) {

  uint16_t probe_count = 0;
  uint16_t index = 0;
  for (uint8_t i = 0; i < grid->length; i++) {
    probe_count += grid->entries[i].probes.length;
    for (uint8_t j = 0; j < grid->entries[i].probes.length; j++) {
      glm_vec3_copy(grid->entries[i].probes.entries[j].position,
                    uniform->entries[index].position);
      index++;
    }
  }

  uniform->length = probe_count;
}

size_t
probe_reflection_grid_list_probe_count(ProbeReflectionGridList *grid_list) {

  size_t count = 0;
  for (size_t i = 0; i < grid_list->length; i++)
    count += grid_list->entries[i].probes.length;

  return count;
}


void probe_reflection_grid_update_boundbox(ProbeReflectionGrid * grid){

  vec3 half;
  glm_vec3_scale(grid->scale, 0.5f, half);

  glm_vec3_add(grid->position, half, grid->boundbox.max);
  glm_vec3_sub(grid->position, half, grid->boundbox.min);
  
  
}
