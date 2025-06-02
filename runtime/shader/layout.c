#include "layout.h"


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

