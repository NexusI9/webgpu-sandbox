#include "./core.h"
#include "../backend/buffer.h"
#include "../runtime/mesh/shader/shader.h"
#include "../utils/system.h"
#include "./global.h"
#include "./local.h"
#include "./texture_list.h"
#include "string.h"
#include "utils.h"
#include "webgpu/webgpu.h"

/**
   Create and cache Ambient Occlusion main array texture.
 */
void ao_bake_init(SceneRendererTextureAO *ao,
                  const AOBakeInitDescriptor *desc) {

  VERBOSE_PROCESS("Initializing Ambient Occlusion Texture...");

  ao->layer_count = desc->layer_count;
  ao->size = desc->size;

  ao->texture = wgpuDeviceCreateTexture(
      desc->device,
      &(WGPUTextureDescriptor){
          .size = {ao->size, ao->size, desc->layer_count},
          .format = AO_TEXTURE_FORMAT,
          .mipLevelCount = 1,
          .sampleCount = 1,
          .dimension = WGPUTextureDimension_2D,
          .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
      });

  ao_bake_texture_list_create(&ao->texture_list, ao->layer_count);
}

void ao_bake_draw_mesh(SceneRendererTextureAO *ao, Mesh *mesh,
                       const AOBakeDrawDescriptor *desc) {

  size_t layer = DYLI_INVALID_INDEX;
  Texture *texture = ao_bake_texture_list_find(&ao->texture_list, mesh, &layer);

  // temp
  if (layer != DYLI_INVALID_INDEX && layer >= (int)ao->layer_count) {
    VERBOSE_WARNING(
        "AO Texture Layer reached max capacity, AO Baking aborted.");
    return;
  }

  if (texture == NULL) {
    AOBakeTextureListEntry *new_entry =
        ao_bake_texture_list_new_entry(&ao->texture_list);

    if (new_entry) {
      new_entry->owner = mesh;
      texture = &new_entry->texture;
      layer = ao->texture_list.length - 1;

      WGPUTextureView layer_view = wgpuTextureCreateView(
          ao->texture, &(WGPUTextureViewDescriptor){
                           .arrayLayerCount = 1,
                           .baseArrayLayer = layer,
                           .mipLevelCount = 1,
                           .aspect = WGPUTextureAspect_All,
                           .baseMipLevel = 0,
                           .dimension = WGPUTextureViewDimension_2D,
                       });

      shader_update_texture_view(mesh_shader_texture(mesh), 0, 8, layer_view,
                                 AO_TEXTURE_FORMAT);

      Shader *shader = mesh_shader_texture(mesh);
      ShaderBindGroup *bind_group = shader_get_bind_group(shader, 0);
      shader_bind_group_refresh(bind_group, 0, desc->device,
                                &shader->pipeline->handle);
    } else {
      VERBOSE_WARNING("New AO texture couldn't be created, AO Bake aborted.");
      return;
    }
  }

  texture_create(texture, &(TextureCreateDescriptor){
                              .width = ao->size,
                              .height = ao->size,
                              .channels = TEXTURE_CHANNELS_R,
                              .value = (uint8_t[]){255},
                          });

  // generate global ao to texture
  if (desc->global.sample_amount)
    TIMER("AO Global Bake", {
      ao_bake_global(ao, &(AOBakeGlobalDescriptor){
                             .device = desc->device,
                             .queue = desc->queue,
                             .mesh_list = desc->mesh_list,
                             .settings = &desc->global,
                             .mesh = mesh,
                             .texture = texture,
                         });
    });

  // generate local ao to texture
  if (desc->local.sample_amount)
    TIMER("AO Global Bake", {
      ao_bake_local(ao, &(AOBakeLocalDescriptor){
                            .device = desc->device,
                            .queue = desc->queue,
                            .settings = &desc->local,
                            .mesh = mesh,
                            .texture = texture,
                        });
    });

  // post process texture (blur, add contrast since sometimes with few sampling
  // factor the dots a too clearly visible)
  ao_bake_process_texture(texture);

  // TODO: batch update ?
  if (layer >= 0 && layer != DYLI_INVALID_INDEX)
    wgpuQueueWriteTexture(desc->queue,
                          &(WGPUImageCopyTexture){
                              .texture = ao->texture,
                              .mipLevel = 0,
                              .origin = {0, 0, layer},
                              .aspect = WGPUTextureAspect_All,
                          },
                          texture->data, texture->size,
                          &(WGPUTextureDataLayout){
                              .offset = 0,
                              .bytesPerRow = texture->width * texture->channels,
                              .rowsPerImage = texture->height,
                          },
                          &(WGPUExtent3D){texture->width, texture->height, 1});
}

void ao_bake_draw_list(SceneRendererTextureAO *ao,
                       const AOBakeDrawDescriptor *desc) {

  for (int t = 0; t < desc->mesh_list->length; t++)
    ao_bake_draw_mesh(ao, desc->mesh_list->entries[t], desc);
}
