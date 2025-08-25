#include "grid.h"
#include "../backend/renderer/scene/std_texture/std_texture.h"
#include "../runtime/mesh/shader/shader.h"
#include "probe.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

static inline float probe_reflection_point(size_t x, uint16_t count,
                                           float size);

static inline void probe_reflection_grid_list_create_texture(
    WGPUTexture *, WGPUTexture *, WGPUTextureView *, WGPUTextureView *,
    const TextureResolution, const WGPUDevice);

float probe_reflection_point(size_t x, uint16_t count, float size) {
  return count > 1 ? ((float)x * 2.0f * size / (count - 1)) - size : 0.0f;
}

void probe_reflection_grid_create(ProbeReflectionGrid *grid,
                                  ProbeReflectionGridDescriptor *desc) {

  glm_vec3_copy(desc->size, grid->size);

  int clamp_count[PROBE_REFLECTION_GRID_DIMENSION]; // prevent overflow

  for (size_t i = 0; i < PROBE_REFLECTION_GRID_DIMENSION; i++)
    clamp_count[i] = glm_min(desc->count[i], PROBE_REFLECTION_GRID_MAX_COUNT);

  glm_ivec3_copy(clamp_count, grid->count);

  size_t count = clamp_count[0] * clamp_count[1] * clamp_count[2];
  probe_reflection_list_create(&grid->probes, count);

  for (size_t x = 0; x < clamp_count[0]; x++) {
    float x_pos = probe_reflection_point(x, clamp_count[0], desc->size[0]);

    for (size_t y = 0; y < clamp_count[1]; y++) {
      float y_pos = probe_reflection_point(y, clamp_count[1], desc->size[1]);

      for (size_t z = 0; z < clamp_count[2]; z++) {
        float z_pos = probe_reflection_point(z, clamp_count[2], desc->size[2]);

        ProbeReflection *probe = probe_reflection_list_new_entry(&grid->probes);

        if (probe)
          glm_vec3_copy((vec3){x_pos, y_pos, z_pos}, probe->position);
      }
    }
  }

  grid->view = std_texture_view(TextureViewType_FloatCubeArray);
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

DynamicListStatus probe_reflection_grid_list_create(
    ProbeReflectionGridList *list,
    const ProbeReflectionGridListDescriptor *desc) {

  DynamicListStatus create =
      dyli_create((void *)&list->entries, &list->capacity, &list->length,
                  sizeof(ProbeReflectionGrid), desc->capacity,
                  "Probe Reflection Grid list");

  if (create == DynamicListStatus_Success) {

    // create list textures array
    WGPUTexture color_texture, depth_texture;
    WGPUTextureView color_view, depth_view;

    probe_reflection_grid_list_create_texture(&color_texture, &depth_texture,
                                              &color_view, &depth_view,
                                              desc->resolution, desc->device);

    // create render pass preset
    render_pass_create(&list->pass,
                       &(RenderPassCreateDescriptor){
                           .label = "Probe Reflection Grid List",
                           .device = desc->device,
                           .queue = desc->queue,
                           .height = desc->resolution,
                           .width = desc->resolution,
                           .draw_list = desc->draw_list,
                           .multisample = desc->multisample,
                           .swapchain = NULL,
                           .color =
                               &(RenderPassColorAttachment){
                                   .clear_value = {0.3f, 0.3f, 0.5f, 1.0f},
                                   .depth_slice = WGPU_DEPTH_SLICE_UNDEFINED,
                                   .load_op = WGPULoadOp_Clear,
                                   .store_op = WGPUStoreOp_Discard,
                                   .texture = color_texture,
                                   .view = color_view,
                               },
                           .depth =
                               &(RenderPassDepthAttachment){
                                   .clear_value = 1.0f,
                                   .load_op = WGPULoadOp_Load, // <== CAUSES FREEZE !!!
                                   .store_op = WGPUStoreOp_Discard,
                                   .read_only = false,
                                   .texture = depth_texture,
                                   .view = depth_view,
                               },
                       });
  }

  return create;
}

void probe_reflection_grid_list_create_texture(
    WGPUTexture *color_texture, WGPUTexture *depth_texture,
    WGPUTextureView *color_view, WGPUTextureView *depth_view,
    const TextureResolution resolution, const WGPUDevice device) {

  /*
    === COLOR ===
   */

  const size_t layer_count =
      PROBE_REFLECTION_GRID_LIST_CAPACITY * PROBE_REFLECTION_LIST_MAX_COUNT;

  *color_texture = wgpuDeviceCreateTexture(
      device,
      &(WGPUTextureDescriptor){
          .label = "Probe Reflection Grid List Texture Color Cube Array",
          .size =
              (WGPUExtent3D){
                  .width = resolution,
                  .height = resolution,
                  .depthOrArrayLayers = layer_count,
              },
          .format = WGPUTextureFormat_BGRA8Unorm,
          .usage = WGPUTextureUsage_CopyDst |
                   WGPUTextureUsage_RenderAttachment |
                   WGPUTextureUsage_TextureBinding,
          .dimension = WGPUTextureDimension_2D,
          .mipLevelCount = 1,
          .sampleCount = 1,
      });

  *color_view = wgpuTextureCreateView(
      *color_texture,
      &(WGPUTextureViewDescriptor){
          .label = "Probe Reflection Grid List View Color Cube Array",
          .format = WGPUTextureFormat_BGRA8Unorm,
          .dimension = WGPUTextureViewDimension_CubeArray,
          .baseMipLevel = 0,
          .mipLevelCount = 1,
          .baseArrayLayer = 0,
          .arrayLayerCount = layer_count,
          .aspect = WGPUTextureAspect_Undefined,
      });

  /*
    === DEPTH ===
   */

  *depth_texture = wgpuDeviceCreateTexture(
      device,
      &(WGPUTextureDescriptor){
          .label = "Probe Reflection Grid List Texture Depth Cube Array",
          .size =
              (WGPUExtent3D){
                  .width = resolution,
                  .height = resolution,
                  .depthOrArrayLayers = layer_count,
              },
          .format = WGPUTextureFormat_Depth24Plus,
          .usage = WGPUTextureUsage_CopyDst |
                   WGPUTextureUsage_RenderAttachment |
                   WGPUTextureUsage_TextureBinding,
          .dimension = WGPUTextureDimension_2D,
          .mipLevelCount = 1,
          .sampleCount = 1,
      });

  *depth_view = wgpuTextureCreateView(
      *depth_texture,
      &(WGPUTextureViewDescriptor){
          .label = "Probe Reflection Grid List View Depth Cube Array",
          .dimension = WGPUTextureViewDimension_CubeArray,
          .format = WGPUTextureFormat_Depth24Plus,
          .baseMipLevel = 0,
          .mipLevelCount = 1,
          .baseArrayLayer = 0,
          .arrayLayerCount = layer_count,
          .aspect = WGPUTextureAspect_DepthOnly,
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

void probe_reflection_grid_list_draw_preprocessor(const RenderPass *pass,
                                                  Mesh *mesh, void *data) {

  ProbeReflectionGridListPreprocessorData *projection =
      (ProbeReflectionGridListPreprocessorData *)data;

  shader_update_uniform(mesh_shader(mesh, MeshShader_Reflection), 0, 0,
                        projection->projection);

  shader_update_uniform(mesh_shader(mesh, MeshShader_Reflection), 0, 1,
                        projection->view);
}

void probe_reflection_grid_list_draw(ProbeReflectionGridList *list) {

  // then update probe list texture cube array based on each probes views
  size_t layer = 0;

  for (size_t i = 0; i < list->length; i++) {

    ProbeReflectionGrid *grid = &list->entries[i];

    TIMER("", {
      VERBOSE_PROCESS("Rendering Probe Reflection Grid %lu/%lu", i + 1,
                      list->length);

      for (size_t j = 0; j < grid->probes.length; j++) {

        ProbeReflection *probe = &grid->probes.entries[j];

        Projection probe_views;
        projection_point(&probe_views, probe->position, 0.1f, 100.0f);

        for (uint8_t k = 0; k < probe_views.length; k++) {

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
              &list->pass, &(ProbeReflectionGridListPreprocessorData){
                               .projection = &probe_views.projection,
                               .view = &probe_views.views[k],
                           });

          // draw pass
          render_pass_draw(&list->pass, &(RenderPassViewOverride){
                                            .color = target_color,
                                            .depth = target_depth,
                                        });

          printf("layer: %lu\n", layer);

          layer++;
        }
      }
    });
  }
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
