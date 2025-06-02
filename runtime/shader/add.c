#include "add.h"
#include "../../backend/buffer.h"
#include "../../utils/system.h"
#include "core.h"
#include "string.h"

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

  if (shader_validate_binding(shader)) {
    /*
      Steps:
        - Increment bind group length
        - Create Buffer (GPU side)
          a. Allocate space in GPU
          b. Write data in buffer
        - Store buffer reference into Uniform object (CPU side)
      */

    ShaderBindGroup *current_bind_group =
        shader_get_bind_group(shader, bd->group_index);

    current_bind_group->visibility = bd->visibility | WGPUShaderStage_Vertex;

    // combine argument entries with uniform buffer
    for (int i = 0; i < bd->entry_count; i++) {

      ShaderBindGroupUniformEntry *current_entry = &bd->entries[i];

      // assign buffer to entry
      buffer_create(
          &bd->entries[i].buffer,
          &(CreateBufferDescriptor){
              .queue = shader->queue,
              .device = shader->device,
              .data = (void *)current_entry->data,
              .size = current_entry->size,
              .usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst,
              .mappedAtCreation = false,
          });

      /*
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
      if (current_entry->update.callback) {
        void *temp_data = current_entry->data;
        current_entry->data = malloc(current_entry->size);
        memcpy(current_entry->data, temp_data, current_entry->size);
      }

      // transfer entry to shader bind group list
      current_bind_group->uniforms
          .entries[current_bind_group->uniforms.length++] = *current_entry;
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

  if (shader_validate_binding(shader)) {
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

      ShaderBindGroupTextureEntry *current_entry = &desc->entries[i];
      // generate texture + texture view from data & size
      buffer_create_texture(&current_entry->texture_view,
                            &(CreateTextureDescriptor){
                                .width = current_entry->width,
                                .height = current_entry->height,
                                .data = current_entry->data,
                                .size = current_entry->size,
                                .device = shader->device,
                                .queue = shader->queue,
                                .format = current_entry->format,
                                .channels = current_entry->channels,
                            });

      current_bind_group->textures
          .entries[current_bind_group->textures.length++] = *current_entry;
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

  if (shader_validate_binding(shader)) {
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
      ShaderBindGroupTextureViewEntry *current_entry = &desc->entries[i];
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

  if (shader_validate_binding(shader)) {
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
      ShaderBindGroupSamplerEntry *current_entry = &desc->entries[i];

      // creating sampler by mapping desc configuration
      current_entry->sampler = wgpuDeviceCreateSampler(
          *shader->device, &(WGPUSamplerDescriptor){
                               .compare = current_entry->compare,
                               .addressModeU = current_entry->addressModeU,
                               .addressModeV = current_entry->addressModeV,
                               .addressModeW = current_entry->addressModeW,
                               .minFilter = current_entry->minFilter,
                               .magFilter = current_entry->magFilter,
                           });

      current_bind_group->samplers
          .entries[current_bind_group->samplers.length++] = *current_entry;
    }
  }
}
