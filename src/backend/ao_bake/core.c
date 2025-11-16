#include "./core.h"

#include "./global.h"
#include "./local.h"
#include "./texture_list.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/resource_manager.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/shader/bindgroup.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "runtime/texture/create.h"
#include "utils.h"
#include "utils/dyli.h"
#include "webgpu/webgpu.h"

/**
   Create and cache Ambient Occlusion main array texture.
 */
void ao_bake_init(RendererTextureAO *ao, const AOBakeInitDescriptor *desc) {

  logger_add(LoggerFlag_Process, "Initializing Ambient Occlusion Texture...");

  ao->layer_count = desc->layer_count;
  ao->size = desc->size;
  ao->texture = rem_new_texture(&(WGPUTextureDescriptor){
      .size = {ao->size, ao->size, desc->layer_count},
      .format = AO_TEXTURE_FORMAT,
      .mipLevelCount = 1,
      .sampleCount = 1,
      .dimension = WGPUTextureDimension_2D,
      .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
  });

  ao_bake_texture_list_create(&ao->texture_list, ao->layer_count);
}

void ao_bake_draw_mesh(RendererTextureAO *ao, Mesh *mesh,
                       const AOBakeDrawDescriptor *desc,
                       bool update_bind_view) {

  size_t layer = DYLI_INVALID_INDEX;
  Texture *texture = ao_bake_texture_list_find(&ao->texture_list, mesh, &layer);

  // temp
  if (layer != DYLI_INVALID_INDEX && layer >= (int)ao->layer_count) {
    logger_add(LoggerFlag_Warning,
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

      WGPUTextureView layer_view = rem_new_view(
          ao->texture, &(WGPUTextureViewDescriptor){
                           .arrayLayerCount = 1,
                           .baseArrayLayer = layer,
                           .mipLevelCount = 1,
                           .aspect = WGPUTextureAspect_All,
                           .baseMipLevel = 0,
                           .dimension = WGPUTextureViewDimension_2D,
                       });

      const bind_group_index AO_group = 1;
      shader_update_texture_view(mesh_shader(mesh, MeshShader_Texture),
                                 AO_group, 8, layer_view, AO_TEXTURE_FORMAT,
                                 ShaderUpdateFlag_ReleasePrevious);

      Shader *shader = mesh_shader(mesh, MeshShader_Texture);
      ShaderBindGroup *bind_group = shader_get_bind_group(shader, AO_group);
      shader_bind_group_refresh(bind_group, AO_group,
                                shader_pipeline(shader)->handle);
    } else {
      logger_add(LoggerFlag_Warning,
                 "New AO texture couldn't be created, AO Bake aborted.");
      return;
    }
  }

  texture_create(texture, &(TextureCreateDescriptor){
                              .width = ao->size,
                              .height = ao->size,
                              .channels = TextureChannel_R,
                              .value = (uint8_t[]){255},
                          });

  // generate global ao to texture
  if (desc->global.sample_amount)
    TIMER("Done", {
      ao_bake_global(ao, &(AOBakeGlobalDescriptor){
                             .mesh_list = desc->mesh_list,
                             .settings = &desc->global,
                             .mesh = mesh,
                             .texture = texture,
                             .debug = desc->debug,
                         });
    });

  // generate local ao to texture
  if (desc->local.sample_amount)
    TIMER("Done", {
      ao_bake_local(ao, &(AOBakeLocalDescriptor){
                            .settings = &desc->local,
                            .mesh = mesh,
                            .texture = texture,
                            .debug = desc->debug,
                        });
    });

  if (update_bind_view) {
    ao_bake_process_texture(texture);
    rem_write_texture(ao->texture, texture->data, texture->size,
                      texture->channels, layer, REMWriteFlag_None);
  }
}

void ao_bake_draw_list(RendererTextureAO *ao,
                       const AOBakeDrawDescriptor *desc) {

  // first compute all texture CPU side
  for (int t = 0; t < desc->mesh_list->length; t++)
    ao_bake_draw_mesh(ao, desc->mesh_list->entries[t], desc, false);

  // once computed, apply post-process and write to GPU
  for (size_t i = 0; i < ao->texture_list.length; i++) {

    Texture *texture = &ao->texture_list.entries[i].texture;

    // post process texture (blur, add contrast since sometimes with few
    // sampling factor the dots a too clearly visible)
    ao_bake_process_texture(texture);
    rem_write_texture(ao->texture, texture->data, texture->size,
                      texture->channels, i, REMWriteFlag_None);
  }
}
