#include "bind.h"


/**
   Initialise shader bind group lists and eventually free/reset the existing
   ones if already existing
 */
void shader_bind_group_init(Shader *shader) {

  // printf("init bind groups for %s\n", shader->name);
  ShaderBindGroupUniforms *uniform_group =
      &shader->bind_groups.entries[shader->bind_groups.length].uniforms;

  ShaderBindGroupTextures *texture_group =
      &shader->bind_groups.entries[shader->bind_groups.length].textures;

  ShaderBindGroupSamplers *sampler_group =
      &shader->bind_groups.entries[shader->bind_groups.length].samplers;

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
}

/**
   Freeing all shader's bind groups allocation and reseting the length
 */
void shader_bind_group_clear(Shader *shader) {

  for (size_t b = 0; b < shader->bind_groups.length; b++) {
    ShaderBindGroup *current_group = &shader->bind_groups.entries[b];
    current_group->bind_group = NULL;
    current_group->index = 0;

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

  int in = 0, i = 0;
  size_t index = shader->bind_groups.length;
  for (i = 0; i < shader->bind_groups.length; i++) {
    // check if group index is already in shader bind group index
    if (group_index == shader->bind_groups.entries[i].index) {
      in = 1;    // true
      index = i; // override group
      break;
    }
  }

  // init new bind group
  if (in == 0) {

    // set index for further references
    index = shader->bind_groups.length;

    // Increment bind_groupd length (push new bind group)
    shader->bind_groups.entries[shader->bind_groups.length].index = group_index;

    // create new bind group
    shader_bind_group_init(shader);

    shader->bind_groups.length++;
  }

  return &shader->bind_groups.entries[index];
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



