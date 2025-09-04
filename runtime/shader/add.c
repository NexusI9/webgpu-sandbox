#include "add.h"
#include "../../backend/buffer.h"
#include "./utils.h"
#include "core.h"
#include "string.h"

#include "../utils/system.h"
#include "webgpu/webgpu.h"

/**
   Add uniform of type Default (vec3, float...) into the shader
 */
void shader_add_uniform(Shader *shader,
                        const ShaderCreateUniformDescriptor *bd) {

  /*
    TODO: Currently we create a buffer per uniform so we don't need to worry
    about the alignment. However this approach is less optimal (since more
    call to GPU) Need to create a way to combine uniforms data into 1 buffer
    and handle the alignment
   */

  if (shader_validate_binding(shader) == SHADER_BIND_VALID) {
    /*
      Steps:
        - Increment bind group length
        - Create Buffer (GPU side)
          a. Allocate space in GPU
          b. Write data in buffer
        - Store buffer reference into Uniform object (CPU side)
      */

    ShaderBindGroup *bind_group =
        shader_get_bind_group(shader, bd->group_index);

    bind_group->visibility = bd->visibility | WGPUShaderStage_Vertex;

    // combine argument entries with uniform buffer
    for (int i = 0; i < bd->entry_count; i++) {

      ShaderBindGroupUniformEntryDescriptor *src = &bd->entries[i];
      ShaderBindGroupUniformEntry *dest =
          &bind_group->uniforms.entries[bind_group->uniforms.length++];

      // copy common attributes
      dest->binding = src->binding;
      dest->size = src->size;
      dest->offset = src->offset;
      dest->data = src->data;
      dest->usage = src->usage;
      dest->update = src->update;

      // assign buffer to entry
      buffer_create(&dest->buffer, &(CreateBufferDescriptor){
                                       .label = "Initial Shader Buffer",
                                       .queue = shader->queue,
                                       .device = shader->device,
                                       .data = (void *)dest->data,
                                       .size = dest->size,
                                       .usage = dest->usage,
                                       .mappedAtCreation = false,
                                   });

      /*

        === CALLBACK HANDLE ===

        Need to dynamically allocate the uniform if it has a callback function,
        cause when we use its pointer during the shader draw process, it
        prevents conflicts if two uniform data have the same address.
        By this we ensure all data have different addresses and prevent
        overwriting conflicts.

        static alloc:
        [mesh_1] uCamera => 0xefd091
        [mesh_2] uCamera => 0xefd091

        dynamic alloc:
        [mesh_1] uCamera => 0xefd092
        [mesh_2] uCamera => 0x48fd23

      */
      if (dest->update.callback) {
        void *temp_data = dest->data;
        dest->data = malloc(dest->size);
        memcpy(dest->data, temp_data, dest->size);
      }

      /*
        === Define dynamic offset ===

        Some shader may use SSBO or UBO global buffer, such approach involve the
        use of dynamic offsets.

        Basically on draw, when the setBindGroup function is called, we provied
        the bindgroup number of entries that use dynamic offset as well as the
        offset list.

        The tricky part is that this array and the indexes are mapped depending
        on the uniforms that have a have the "hasDynamicOffset" set to true.
        Meaning if you have:

          uni_1 : true
          uni_2 : false        =======>     [uni_1, uni_3]  (no uni_2)
          uni_3 : true

        So when we apply the offset, we can't simply use the inform binding as
        index since the setBindGroup only lookup dynamic uniforms.

        As a result if the uniform buffer is marked as "hasDynamicOffset" we
        link the bindgroup offset list relative index pointer to directly update
        it later.

                   .-------- index 0 * --------.
                   v                           |
          uni_1 : true                         |
          uni_2 : false        =======>     [uni_1, uni_3]
          uni_3 : true                                |
                   ^                                  |
                   '-----------index 1 * -------------'

       */

      if (src->hasDynamicOffset &&
          bind_group->offset.count < SHADER_MAX_OFFSET_CAPACITY) {

        dest->dynamic_offset_entry =
            &bind_group->offset.entries[bind_group->offset.count++];
      }
    }
  }
}

/**
   Add uniform of type Texture into the shader
   The function upload the texture to the buffer
   and automatically handles the texture view creation.

   This function is usefull in case one want to upload and bind
   a texture from "raw data" when reading a file from disk (stbi, gltf...)
 */
void shader_add_texture(Shader *shader,
                        const ShaderCreateTextureDescriptor *desc) {

  if (shader_validate_binding(shader) == SHADER_BIND_VALID) {
    ShaderBindGroup *current_bind_group =
        shader_get_bind_group(shader, desc->group_index);

    current_bind_group->visibility =
        desc->visibility | WGPUShaderStage_Fragment;

    for (int i = 0; i < desc->entry_count; i++) {

      if (current_bind_group->textures.length ==
          current_bind_group->textures.capacity) {
        VERBOSE_PRINT("Texture array reached maximum capacity\n");
        break;
      }

      ShaderBindGroupTextureEntryDescriptor *src = &desc->entries[i];
      ShaderBindGroupTextureEntry *dest =
          &current_bind_group->textures
               .entries[current_bind_group->textures.length++];

      // copy common attributes
      dest->binding = src->binding;
      dest->channels = src->channels;
      dest->data = src->data;
      dest->dimension = src->dimension;
      dest->sample_type = src->sample_type;
      dest->format = src->format;
      dest->width = src->width;
      dest->height = src->height;
      dest->size = src->size;

      // generate texture + texture view from data & size
      buffer_create_texture(&dest->texture_view,
                            &(CreateTextureDescriptor){
                                .width = dest->width,
                                .height = dest->height,
                                .data = dest->data,
                                .size = dest->size,
                                .device = shader->device,
                                .queue = shader->queue,
                                .format = dest->format,
                                .channels = dest->channels,
                            },
                            BufferTextureMemory_Free);
    }
  }
}

/**
   Add uniform of type Texture into the shader
   The function takes a "ready" texture view with a
   valid and already uploaded texture.

   In case one want to bind a raw picture/data (let's say imported from a file),
   one shall use the shader_add_texture() function that automatically
   handles the texture and texture view creation from the data.
 */
void shader_add_texture_view(Shader *shader,
                             const ShaderCreateTextureViewDescriptor *desc) {

  if (shader_validate_binding(shader) == SHADER_BIND_VALID) {
    ShaderBindGroup *current_bind_group =
        shader_get_bind_group(shader, desc->group_index);

    current_bind_group->visibility =
        desc->visibility | WGPUShaderStage_Fragment;

    for (int i = 0; i < desc->entry_count; i++) {

      if (current_bind_group->textures.length ==
          current_bind_group->textures.capacity) {
        VERBOSE_PRINT("Texture array reached maximum capacity\n");
        break;
      }

      // map the entry to bind group
      ShaderBindGroupTextureViewEntryDescriptor *current_entry =
          &desc->entries[i];
      current_bind_group->textures
          .entries[current_bind_group->textures.length++] =
          (ShaderBindGroupTextureEntry){
              .texture_view = current_entry->texture_view,
              .binding = current_entry->binding,
              .dimension = current_entry->dimension,
              .format = current_entry->format,
              .sample_type = current_entry->sample_type,
          };
    }
  }
}

/**
   Add uniform of type Sampler into the shader
 */
void shader_add_sampler(Shader *shader,
                        const ShaderCreateSamplerDescriptor *desc) {

  if (shader_validate_binding(shader) == SHADER_BIND_VALID) {
    ShaderBindGroup *current_bind_group =
        shader_get_bind_group(shader, desc->group_index);

    current_bind_group->visibility =
        desc->visibility | WGPUShaderStage_Fragment;

    for (int i = 0; i < desc->entry_count; i++) {

      if (current_bind_group->samplers.length ==
          current_bind_group->samplers.capacity) {
        VERBOSE_PRINT("Sampler array reached maximum capacity\n");
        break;
      }

      // generate texture + sampler + texture view from data & size
      ShaderBindGroupSamplerEntryDescriptor *src = &desc->entries[i];
      ShaderBindGroupSamplerEntry *dest =
          &current_bind_group->samplers
               .entries[current_bind_group->samplers.length++];

      // copy common attributes
      dest->binding = src->binding;
      dest->addressModeU = src->addressModeU;
      dest->addressModeV = src->addressModeV;
      dest->addressModeW = src->addressModeW;
      dest->minFilter = src->minFilter;
      dest->magFilter = src->magFilter;
      dest->type = src->type;
      dest->compare = src->compare;

      // creating sampler by mapping desc configuration
      dest->sampler = wgpuDeviceCreateSampler(
          shader->device, &(WGPUSamplerDescriptor){
                              .compare = dest->compare,
                              .addressModeU = dest->addressModeU,
                              .addressModeV = dest->addressModeV,
                              .addressModeW = dest->addressModeW,
                              .minFilter = dest->minFilter,
                              .magFilter = dest->magFilter,
                          });
    }
  }
}
