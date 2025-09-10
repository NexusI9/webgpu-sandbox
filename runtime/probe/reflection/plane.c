#include "plane.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

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
              .view_offset = probe->ssbo_slot[ProbeReflectionSSBOField_View].id,
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
  probe->ref_camera = desc->camera;
  probe->signed_distance = glm_dot(probe->normal, probe->position);

  // create "fake camera" that will actually just copy the reference camera
  // reflected position/ angle, thus we don't need to pass "sensitivy"/ "mode"
  // attributes
  camera_create(&probe->camera, &(CameraCreateDescriptor){0});

  // Allocate init shader attribute (uniform/ view)
  ssbo_slot_init_alloc(&probe->ssbo_slot[ProbeReflectionSSBOField_View],
                       sizeof(CameraUniform));

  probe_reflection_plane_update_camera(probe);

  ssbo_slot_init_alloc(&probe->ssbo_slot[ProbeReflectionSSBOField_List],
                       sizeof(ProbeReflectionPlaneUniform));

  probe_reflection_plane_update_uniform(probe);
  probe_reflection_plane_update_boundbox(probe);
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

  uniform->camera_ssbo_index =
      probe->ssbo_slot[ProbeReflectionSSBOField_View].id;
  uniform->near = probe->near;
  uniform->far = probe->far;
  uniform->distance = probe->distance;
  uniform->signed_distance = probe->signed_distance;
}

void probe_reflection_plane_update_camera(ProbeReflectionPlane *probe) {

  // transfert the reference camera reflected attributes to the probes shallow
  // camera
  vec3_reflect_point((float *)probe->ref_camera->position,
                     (float *)probe->normal, probe->signed_distance,
                     probe->camera.position);

  vec3_reflect_dir((float *)probe->ref_camera->forward, (float *)probe->normal,
                   probe->camera.forward);

  vec3_reflect_dir((float *)probe->ref_camera->up, (float *)probe->normal,
                   probe->camera.up);

  glm_vec3_add(probe->camera.position, probe->camera.forward,
               probe->camera.target);

  glm_lookat(probe->camera.position, probe->camera.target, probe->camera.up,
             probe->camera.view);

  camera_uniform_update(&probe->camera);

  // transfert attribute to SSBO slot
  CameraUniform *uniform = camera_uniform(&probe->camera);
  ssbo_slot_set_uniform(&probe->ssbo_slot[ProbeReflectionSSBOField_View],
                        (void *)uniform, sizeof(CameraUniform));
}

void probe_reflection_plane_update_boundbox(ProbeReflectionPlane *probe) {

  vec3 half = {
      probe->scale[0] * 0.5f,
      probe->distance * 0.5f,
      probe->scale[1] * 0.5f,
  };

  // if probe is aligned to axis (faster that oriented)
  if (fabsf(glm_vec3_dot(probe->normal, (vec3){1.0f, 0.0f, 0.0f})) > 0.999f ||
      fabsf(glm_vec3_dot(probe->normal, (vec3){0.0f, 1.0f, 0.0f})) > 0.999f ||
      fabsf(glm_vec3_dot(probe->normal, (vec3){0.0f, 0.0f, 1.0f})) > 0.999f) {

    glm_vec3_copy(probe->position, probe->boundbox.min);
    glm_vec3_copy(probe->position, probe->boundbox.max);

    glm_vec3_sub(probe->position, half, probe->boundbox.min);
    glm_vec3_add(probe->position, half, probe->boundbox.max);

  } else {

    vec3 inv_half;
    glm_vec3_scale(half, -1.0f, inv_half);

    // build transform matrix for TBN
    mat3 tbn;

    glm_vec3_copy(probe->tangent, tbn[0]);
    glm_vec3_copy(probe->bitangent, tbn[1]);
    glm_vec3_copy(probe->normal, tbn[2]);

    mat4 matrix;
    glm_mat4_identity(matrix);
    glm_mat4_ins3(tbn, matrix);
    glm_translate(matrix, probe->position);

    glm_vec3_copy((vec3){FLT_MAX, FLT_MAX, FLT_MAX}, probe->boundbox.min);
    glm_vec3_copy((vec3){FLT_MIN, FLT_MIN, FLT_MIN}, probe->boundbox.max);

    for (uint8_t x = 0; x < 2; x++) {
      for (uint8_t y = 0; y < 2; y++) {
        for (uint8_t z = 0; z < 2; z++) {

          vec3 local_corner = {
              x ? half[0] : inv_half[0],
              y ? half[1] : inv_half[1],
              z ? half[2] : inv_half[2],
          };

          vec3 world_corner;
          glm_mat4_mulv3(matrix, local_corner, 1.0f, world_corner);

          glm_vec3_minv(probe->boundbox.min, world_corner, probe->boundbox.min);
          glm_vec3_minv(probe->boundbox.max, world_corner, probe->boundbox.max);
        }
      }
    }
  }
}
