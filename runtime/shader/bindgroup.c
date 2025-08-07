#include "bindgroup.h"
#include "../utils/system.h"
#include "./utils.h"
#include "core.h"
#include <stdint.h>
#include <string.h>

static inline void shader_convert_uniforms(ShaderBindGroup *,
                                           WGPUBindGroupEntry *, bind_index *);
static inline void shader_convert_textures(ShaderBindGroup *,
                                           WGPUBindGroupEntry *, bind_index *);
static inline void shader_convert_samplers(ShaderBindGroup *,
                                           WGPUBindGroupEntry *, bind_index *);

/**
   Initialise shader bind group lists and eventually free/reset the existing
   ones if already existing.

    Max stack allocation easily reached with static allocation for
    Uniforms, Texture and Sampler arrays, so need to allocate them on the heap.
 */
void shader_bind_group_create(Shader *shader, bind_group_index index) {

  if (index > SHADER_MAX_BIND_GROUP) {
    VERBOSE_WARNING("Cannot initialize a group index > %d.",
                    SHADER_MAX_BIND_GROUP);
    return;
  }

  ShaderBindGroupUniforms *uniform_group =
      &shader->bind_groups.entries[index].uniforms;

  ShaderBindGroupTextures *texture_group =
      &shader->bind_groups.entries[index].textures;

  ShaderBindGroupSamplers *sampler_group =
      &shader->bind_groups.entries[index].samplers;

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

  shader->bind_groups.length++;
}

/**
   Freeing all shader's bind groups allocation and reseting the length
 */
void shader_bind_group_clear(Shader *shader) {

  for (size_t b = 0; b < shader->bind_groups.length; b++) {
    ShaderBindGroup *current_group = &shader->bind_groups.entries[b];

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

    wgpuBindGroupRelease(current_group->bind_group);
    current_group->bind_group = NULL;
  }

  shader->bind_groups.length = 0;
}

void shader_convert_uniforms(ShaderBindGroup *bindgroup,
                             WGPUBindGroupEntry *entries, bind_index *index) {

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

void shader_convert_textures(ShaderBindGroup *bindgroup,
                             WGPUBindGroupEntry *entries, bind_index *index) {

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

void shader_convert_samplers(ShaderBindGroup *bindgroup,
                             WGPUBindGroupEntry *entries, bind_index *index) {

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
ShaderBindGroup *shader_get_bind_group(Shader *shader,
                                       bind_group_index group_index) {

  // check if group within acceptable range
  if (group_index >= SHADER_MAX_BIND_GROUP) {
    VERBOSE_ERROR("WebGPU is unable to create more than 4 bind groups.");
    return NULL;
  }

  // check if group index already exists
  if (shader->bind_groups.entries[group_index].textures.entries == NULL) {
    // create new bind group
    shader_bind_group_create(shader, group_index);
  }

  return &shader->bind_groups.entries[group_index];
}

/**
   Convert the ShaderBindGroup into WGPUBindGroup.
   The converted entries are then realized.
 */
WGPUBindGroupEntry *shader_bind_group_convert(ShaderBindGroup *group) {

  uint16_t total_length = shader_bind_group_entries_count(group);

  WGPUBindGroupEntry *converted_entries =
      (WGPUBindGroupEntry *)malloc(total_length * sizeof(WGPUBindGroupEntry));

  uint16_t length = 0;
  // bind uniforms
  shader_convert_uniforms(group, converted_entries, &length);
  // bind textures
  shader_convert_textures(group, converted_entries, &length);
  // bind samplers
  shader_convert_samplers(group, converted_entries, &length);

  return converted_entries;
}

void shader_bind_group_realize(WGPUBindGroup *bind_group,
                               const ShaderBindGroupRealize *desc) {

  *bind_group = wgpuDeviceCreateBindGroup(
      desc->device, &(WGPUBindGroupDescriptor){
                        .layout = wgpuRenderPipelineGetBindGroupLayout(
                            *desc->pipeline_handle, desc->group_index),
                        .entryCount = desc->entryCount,
                        .entries = desc->entries,
                    });
}

void shader_bind_group_release(ShaderBindGroup *shader_bind_group) {
  if (shader_bind_group->bind_group != NULL)
    wgpuBindGroupRelease(shader_bind_group->bind_group);
}

/**
   III. Last phase of the shader build process.

   In the previous steps we:
   1. Organise the layouts.
   2. Apply the layouts in the pipeline.

   Howerver the pipeline only own a layout/ blueprint an not the content itself.
   Meaning we still have to upload the actual bindgroup content to use it in the
   shader. That's the purpose of this latest phase.

   Overall flow:
   1. Traverse all uniforms/textures and samplers from a bind group
   2. Convert entries (tex/smpl/unfrm) to generic "WGPUBindGroupEntry"
   3. Store those converted entries in an array or entry.
   4. Create bind group based on the converted entries.
 */
void shader_bind_group_build(ShaderBindGroup *group,
                             bind_group_index group_index,
                             const WGPUDevice device,
                             const WGPURenderPipeline *pipeline) {

  uint16_t total_length = shader_bind_group_entries_count(group);

#ifdef VERBOSE_BINDING_PHASE
  VERBOSE_PRINT("\t\t\t└ Uniforms: %lu\n\t\t\t└ Textures: "
                "%lu\n\t\t\t└ Samplers: %lu",
                group->uniforms.length, group->textures.length,
                group->samplers.length);
#endif

  // convert shader bind group to WGPU bind group
  WGPUBindGroupEntry *converted_entries = shader_bind_group_convert(group);

  // realize bind group
  shader_bind_group_realize(&group->bind_group,
                            &(ShaderBindGroupRealize){
                                .group_index = group_index,
                                .device = device,
                                .entryCount = total_length,
                                .entries = converted_entries,
                                .pipeline_handle = pipeline,
                            });

  // release layouts
  free(converted_entries);
  // TODO: Clear layouts on mesh destruction
  // WGPUBindGroupLayout *current_layout = &layouts[i];
  // wgpuBindGroupLayoutRelease(*current_layout);
}
