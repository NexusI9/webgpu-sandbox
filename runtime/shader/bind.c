#include "bind.h"
#include <string.h>

/**
   Initialise shader bind group lists and eventually free/reset the existing
   ones if already existing
 */
void shader_bind_group_init(Shader *shader, size_t index) {

  if (index > SHADER_MAX_BIND_GROUP) {
    perror("Cannot initialize a group index > 4\n");
    return;
  }

  // printf("init bind groups for %s\n", shader->name);
  ShaderBindGroupUniforms *uniform_group =
      &shader->bind_groups.entries[index].uniforms;

  ShaderBindGroupTextures *texture_group =
      &shader->bind_groups.entries[index].textures;

  ShaderBindGroupSamplers *sampler_group =
      &shader->bind_groups.entries[index].samplers;

  /*NOTE:
    Max stack allocation easily reached with static allocation for
    Uniforms, Texture and Sampler arrays, so need to allocate them on the heap
  */

  // init Uniforms dynamic array
  uniform_group->length = 0;
  uniform_group->capacity = SHADER_UNIFORMS_DEFAULT_CAPACITY;
  uniform_group->entries = (ShaderBindGroupUniformEntry *)aligned_alloc(
      16,
      SHADER_UNIFORMS_DEFAULT_CAPACITY * sizeof(ShaderBindGroupUniformEntry));

  /*memset(uniform_group->entries, 0,
         SHADER_UNIFORMS_DEFAULT_CAPACITY *
         sizeof(ShaderBindGroupUniformEntry));*/

  // init Texture dynamic array
  texture_group->length = 0;
  texture_group->capacity = SHADER_UNIFORMS_DEFAULT_CAPACITY;
  texture_group->entries = (ShaderBindGroupTextureEntry *)aligned_alloc(
      16,
      SHADER_UNIFORMS_DEFAULT_CAPACITY * sizeof(ShaderBindGroupTextureEntry));

  // init Sampler dynamic array
  sampler_group->length = 0;
  sampler_group->capacity = SHADER_UNIFORMS_DEFAULT_CAPACITY;
  sampler_group->entries = (ShaderBindGroupSamplerEntry *)aligned_alloc(
      16,
      SHADER_UNIFORMS_DEFAULT_CAPACITY * sizeof(ShaderBindGroupSamplerEntry));

  shader->bind_groups.length++;
}

/**
   Freeing all shader's bind groups allocation and reseting the length
 */
void shader_bind_group_clear(Shader *shader) {

  for (size_t b = 0; b < shader->bind_groups.length; b++) {
    ShaderBindGroup *current_group = &shader->bind_groups.entries[b];
    current_group->bind_group = NULL;

    // reseting uniforms
    current_group->uniforms.length = 0;

    // reseting textures
    if (current_group->textures.entries)
      free(current_group->textures.entries);

    current_group->textures.entries = NULL;
    current_group->textures.length = 0;
    current_group->textures.capacity = 0;

    // reseting samplers
    if (current_group->samplers.entries)
      free(current_group->samplers.entries);

    current_group->samplers.entries = NULL;
    current_group->samplers.length = 0;
    current_group->samplers.capacity = 0;
  }

  shader->bind_groups.length = 0;
}

void shader_bind_uniforms(Shader *shader, ShaderBindGroup *bindgroup,
                          WGPUBindGroupEntry *entries, uint16_t *index) {

  // map shader bind group entry to WGPU bind group entry
  // (basically the same just without data and callback attributes)
  for (int j = 0; j < bindgroup->uniforms.length; j++) {
    ShaderBindGroupUniformEntry *current_entry =
        &bindgroup->uniforms.entries[j];
    entries[(*index)++] = (WGPUBindGroupEntry){
        .binding = current_entry->binding,
        .buffer = current_entry->buffer,
        .offset = current_entry->offset,
        .size = current_entry->size,
    };
  }
}

void shader_bind_textures(Shader *shader, ShaderBindGroup *bindgroup,
                          WGPUBindGroupEntry *entries, uint16_t *index) {

  // map shader bind group entry to WGPU bind group entry
  // (basically the same just without data and callback attributes)
  for (int j = 0; j < bindgroup->textures.length; j++) {
    ShaderBindGroupTextureEntry *current_entry =
        &bindgroup->textures.entries[j];
    entries[(*index)++] = (WGPUBindGroupEntry){
        .binding = current_entry->binding,
        .textureView = current_entry->texture_view,
    };
  }
}

void shader_bind_samplers(Shader *shader, ShaderBindGroup *bindgroup,
                          WGPUBindGroupEntry *entries, uint16_t *index) {

  for (int j = 0; j < bindgroup->samplers.length; j++) {
    ShaderBindGroupSamplerEntry *current_entry =
        &bindgroup->samplers.entries[j];
    entries[(*index)++] = (WGPUBindGroupEntry){
        .binding = current_entry->binding,
        .sampler = current_entry->sampler,
    };
  }
}

/**
   Check if a bind group in the shader isn't already registered
   if not, it creates a new bind group entry to the list
 */
ShaderBindGroup *shader_get_bind_group(Shader *shader, size_t group_index) {

  // check if group within acceptable range
  if (group_index >= SHADER_MAX_BIND_GROUP) {
    perror("WebGPU is unable to create more than 4 bind groups\n");
    return NULL;
  }

  // check if group index already exists
  if (shader->bind_groups.entries[group_index].textures.entries == NULL) {
    // create new bind group
    shader_bind_group_init(shader, group_index);
  }

  return &shader->bind_groups.entries[group_index];
}

/**
   Check if the shader has every requirements before binding groups
   Also checks if the bind groups array isn't already at full capacity
 */

// TODO add more validation by uniforms type (UNIFORM/ TEX/ SAMPLER...) check
// if it doesn't overflow with max accepted length
bool shader_validate_binding(Shader *shader) {

  if (shader->device == NULL || shader->queue == NULL) {
    perror("Shader has no device or queue");
    return 0;
  }

  if (shader->bind_groups.length >= SHADER_MAX_BIND_GROUP) {
    perror("Bind group list at full capacity");
    return 0;
  }

  return 1;
}
