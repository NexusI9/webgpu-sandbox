#include "layout.h"
#include "./utils.h"

#include "../utils/system.h"

static inline void shader_layout_uniforms(Shader *, ShaderBindGroup *,
                                          WGPUBindGroupLayoutEntry *,
                                          uint16_t *);
static inline void shader_layout_textures(Shader *, ShaderBindGroup *,
                                          WGPUBindGroupLayoutEntry *,
                                          uint16_t *);
static inline void shader_layout_samplers(Shader *, ShaderBindGroup *,
                                          WGPUBindGroupLayoutEntry *,
                                          uint16_t *);

/**
   I. First process of the shader building phase.

   We need to first define bind group layout before actually pushing values in
   it.

   Divide the uniforms type if different classes (uniforms/ textures/
   sampler) as they require dedicated layouts.

   Layouts define in a higher level
   what the GPU expects in term of type and structure

    .----------.      +===========+     .---------.     .----------.
    |  SHADER  | ==> || PIPELINE || <== | LAYOUTS | <== | UNIFORMS |
    '----------'     +===========+      '---------'     '----------'
        GPU                                                 CPU

  */
WGPUBindGroupLayout *shader_layout_build(Shader *shader) {

  // need to use malloc cause of VLA (variable length array)
  WGPUBindGroupLayout *layout_list = (WGPUBindGroupLayout *)malloc(
      shader->bind_groups.length * sizeof(WGPUBindGroupLayout));

  // go through shader bind groups and combine entries
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_group = &shader->bind_groups.entries[i];
    WGPUBindGroupLayout *current_layout = &layout_list[i];

    // combine all bind group entries in one array
    uint16_t total_length = shader_bind_group_entries_count(current_group);

    uint16_t length = 0;

    WGPUBindGroupLayoutEntry *layout_entries =
        (WGPUBindGroupLayoutEntry *)malloc(total_length *
                                           sizeof(WGPUBindGroupLayoutEntry));

    // layout uniforms
    shader_layout_uniforms(shader, current_group, layout_entries, &length);
    // layout textures
    shader_layout_textures(shader, current_group, layout_entries, &length);
    // layout samplers
    shader_layout_samplers(shader, current_group, layout_entries, &length);

    // create layout from previously populated entries array
    *current_layout = wgpuDeviceCreateBindGroupLayout(
        shader->device, &(WGPUBindGroupLayoutDescriptor){
                            .entryCount = total_length,
                            .entries = layout_entries,
                        });

    free(layout_entries);
  }

  return layout_list;
}

void shader_layout_uniforms(Shader *shader, ShaderBindGroup *bindgroup,
                            WGPUBindGroupLayoutEntry *entries,
                            uint16_t *length) {

  ShaderBindGroupUniforms *uniform_entries = &bindgroup->uniforms;

  // go through each entries
  for (int j = 0; j < uniform_entries->length; j++) {
    entries[(*length)++] = (WGPUBindGroupLayoutEntry){
        // assign stored binding index
        .binding = uniform_entries->entries[j].binding,
        // buffer binding layout
        .buffer = {.type = WGPUBufferBindingType_Uniform},
        // set visibility to vertex
        .visibility = bindgroup->visibility,
    };
  }
}

void shader_layout_textures(Shader *shader, ShaderBindGroup *bindgroup,
                            WGPUBindGroupLayoutEntry *entries,
                            uint16_t *length) {

  ShaderBindGroupTextures *texture_entries = &bindgroup->textures;

  // go through each entries
  for (int j = 0; j < texture_entries->length; j++) {
    ShaderBindGroupTextureEntry *current_entry = &texture_entries->entries[j];
    entries[(*length)++] = (WGPUBindGroupLayoutEntry){
        .texture =
            {
                .sampleType = current_entry->sample_type,
                .viewDimension = current_entry->dimension,
            },
        .binding = current_entry->binding,
        .visibility = bindgroup->visibility,
    };
  }
}

void shader_layout_samplers(Shader *shader, ShaderBindGroup *bindgroup,
                            WGPUBindGroupLayoutEntry *entries,
                            uint16_t *length) {

  ShaderBindGroupSamplers *sampler_entries = &bindgroup->samplers;

  // go through each entries
  for (int j = 0; j < sampler_entries->length; j++) {
    ShaderBindGroupSamplerEntry *current_entry = &sampler_entries->entries[j];
    entries[(*length)++] = (WGPUBindGroupLayoutEntry){
        .sampler = {.type = current_entry->type},
        .binding = sampler_entries->entries[j].binding,
        .visibility = bindgroup->visibility,
    };
  }
}
