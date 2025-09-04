#include "plane.h"
#include "core.h"
#include "webgpu/webgpu.h"

DynamicListStatus
probe_reflection_plane_list_create(ProbeReflectionPlaneList *list,
                                   const ProbeReflectionListDescriptor *desc) {

  return probe_reflection_list_create_core(&(ProbeReflectionCreateCore){
      .device = desc->device,
      .queue = desc->queue,
      .probe_list =
          &(ProbeReflectionCreateCoreList){
              .entries = (void *)&list->entries,
              .capacity = &list->capacity,
              .length = &list->length,
              .type_size = sizeof(ProbeReflectionPlane),
              .label = "Probe Reflection Plane list",
              .num = desc->capacity,
          },
      .render_pass =
          &(ProbeReflectionCreateCorePass){
              .draw_list = desc->draw_list,
              .handle = &list->pass,
              .view_dimension = WGPUTextureViewDimension_2DArray,
              .resolution = desc->resolution,
              .multisample = desc->multisample,
              .layer_count = PROBE_REFLECTION_PLANE_LIST_LAYER_COUNT,
          },
  });
}

DynamicListStatus
probe_reflection_plane_list_insert(ProbeReflectionPlaneList *list,
                                   ProbeReflectionPlane *entry) {

  // temporary (shader only accept static array for now)
  if (list->length == PROBE_REFLECTION_PLANE_LIST_LAYER_COUNT)
    return DynamicListStatus_UndefError;

  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(ProbeReflectionPlane), (void *)entry, 1,
                     "Probe Reflection Plane list");
}

ProbeReflectionPlane *
probe_reflection_plane_list_new_entry(ProbeReflectionPlaneList *list) {

  // temporary (shader only accept static array for now)
  if (list->length == PROBE_REFLECTION_PLANE_LIST_LAYER_COUNT)
    return NULL;

  return (ProbeReflectionPlane *)dyli_new_entry(
      (void *)&list->entries, &list->capacity, &list->length,
      sizeof(ProbeReflectionPlane), "Probe Reflection Grid list");
}

DynamicListStatus
probe_reflection_plane_list_remove(ProbeReflectionPlaneList *list,
                                   ProbeReflectionPlane *entry) {
  return dyli_remove((void *)list->entries, &list->length,
                     sizeof(ProbeReflectionPlane), (void *)entry,
                     "Probe Reflection Plane list");
}

DynamicListStatus
probe_reflection_plane_list_destroy(ProbeReflectionPlaneList *list) {
  return dyli_free((void *)list->entries, &list->capacity, &list->length);
}

void probe_reflection_plane_list_draw_callback(void *data) {

  // temp
  ProbeReflectionListDebug *debug = NULL;
  ProbeReflectionPlaneList *list = (ProbeReflectionPlaneList *)data;

  // then update probe list texture cube array based on each probes views
  size_t layer = 0;

  render_pass_command_begin(&list->pass);
  {
    for (size_t i = 0; i < list->length; i++) {

      ProbeReflectionPlane *probe = &list->entries[i];

      Projection *views = &probe->views;

      for (uint8_t k = 0; k < views->length; k++) {

        // define target layer
        WGPUTextureView target_color = wgpuTextureCreateView(
            list->pass.color.texture,
            &(WGPUTextureViewDescriptor){
                .label = "Probe Reflection Plane Target Color View",
                .arrayLayerCount = 1,
                .baseArrayLayer = layer,
                .dimension = WGPUTextureViewDimension_2D,
                .baseMipLevel = 0,
                .mipLevelCount = 1,
            });

        WGPUTextureView target_depth = wgpuTextureCreateView(
            list->pass.depth.texture,
            &(WGPUTextureViewDescriptor){
                .label = "Probe Reflection Plane Target Depth View",
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
                    probe->ssbo_slot[ProbeReflectionSSBOField_View].id,
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
  }
  render_pass_command_end(&list->pass);
}

void probe_reflection_plane_create(ProbeReflectionPlane *probe,
                                   ProbeReflectionPlaneDescriptor *desc) {

  // Define init attribute
  glm_vec3_copy(desc->position, probe->position);
  glm_vec3_copy(desc->scale, probe->scale);
  glm_vec3_copy(desc->normal, probe->normal);

  vec3_tangent(probe->normal, probe->tangent);
  vec3_bitangent(probe->normal, probe->tangent, probe->bitangent);

  probe->near = desc->near;
  probe->far = desc->far;
  probe->distance = desc->distance;
  probe->camera = desc->camera;
  probe->viewport = desc->viewport;
  probe->signed_distance = glm_dot(probe->normal, probe->position);

  // Allocate init shader attribute (uniform/ view)
  ssbo_slot_init_alloc(&probe->ssbo_slot[ProbeReflectionSSBOField_List],
                       sizeof(ProbeReflectionPlaneUniform));

  probe_reflection_plane_update_uniform(probe);

  ssbo_slot_init_alloc(&probe->ssbo_slot[ProbeReflectionSSBOField_View],
                       sizeof(ProjectionUniform));

  probe_reflection_plane_update_view(probe);
}

void probe_reflection_plane_update_uniform(ProbeReflectionPlane *probe) {

  ProbeReflectionPlaneUniform *uniform =
      (ProbeReflectionPlaneUniform *)probe
          ->ssbo_slot[ProbeReflectionSSBOField_List]
          .uniform;

  glm_vec3_copy(probe->position, uniform->position);
  glm_vec3_copy(probe->scale, uniform->scale);
  glm_vec3_copy(probe->normal, uniform->normal);
  glm_vec3_copy(probe->tangent, uniform->tangent);
  glm_vec3_copy(probe->bitangent, uniform->bitangent);

  uniform->near = probe->near;
  uniform->far = probe->far;
  uniform->distance = probe->distance;
  uniform->signed_distance = probe->signed_distance;
}

void probe_reflection_plane_update_view(ProbeReflectionPlane *probe) {
  // update light projection attribute
  projection_mirror(&probe->views, probe->normal, probe->signed_distance,
                    probe->camera, probe->viewport);

  // transfert attribute to SSBO slot
  projection_update_ssbo_slot(probe->ssbo_slot, &probe->views,
                              ProbeReflectionSSBOField_View);
}
