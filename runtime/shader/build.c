#include "build.h"
#include "bind.h"
#include "layout.h"

/**
   Build pipeline based on previously set bind groups
 */
void shader_build(Shader *shader) {

  // clear pipeline if existing
#ifdef VERBOSE_BUILDING_PHASE
  VERBOSE_PRINT("  └ Building Shader: %s\n", shader->name);
#endif

  // build bind group entries for each individual group index
  WGPUBindGroupLayout *bindgroup_layouts = shader_build_layout(shader);
  shader_build_pipeline(shader, bindgroup_layouts);
  shader_build_bind(shader, bindgroup_layouts);

  // shader_module_release(shader);
  shader_pipeline_release_layout(shader);
  free(bindgroup_layouts);

  // update uniforms first

}

WGPUBindGroupLayout *shader_build_layout(Shader *shader) {

  /*
    need to first define bind group layout before actually pushing values in it
    divide the uniforms type if different classes (uniforms/ textures/ sampler)
    as they require dedicated layouts.
    Layouts define in a higher level what the GPU expects in term of type and
    structure

    .----------.      +===========+     .---------.     .----------.
    |  SHADER  | ==> || PIPELINE || <== | LAYOUTS | <== | UNIFORMS |
    '----------'     +===========+      '---------'     '----------'
        GPU                                                 CPU

  */

  // need to use malloc cause of VLA (variable length array)
  WGPUBindGroupLayout *layout_list = (WGPUBindGroupLayout *)malloc(
      shader->bind_groups.length * sizeof(WGPUBindGroupLayout));

  // go through shader bind groups and combine entries
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_group = &shader->bind_groups.entries[i];
    WGPUBindGroupLayout *current_layout = &layout_list[i];

    // combine all bind group entries in one array
    uint16_t total_length = current_group->uniforms.length +
                            current_group->textures.length +
                            current_group->samplers.length;

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
        *shader->device, &(WGPUBindGroupLayoutDescriptor){
                             .entryCount = total_length,
                             .entries = layout_entries,
                         });

    free(layout_entries);
  }

  return layout_list;
}

void shader_build_pipeline(Shader *shader, WGPUBindGroupLayout *layout) {

  WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
      *shader->device, &(WGPUPipelineLayoutDescriptor){
                           // total bind groups count
                           .bindGroupLayoutCount = shader->bind_groups.length,
                           .bindGroupLayouts = layout,
                           .label = shader->name,
                       });

  // create pipeline
  pipeline_build(&shader->pipeline, &pipeline_layout);
}

void shader_build_bind(Shader *shader, WGPUBindGroupLayout *layouts) {

  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_group = &shader->bind_groups.entries[i];
    WGPUBindGroupLayout *current_layout = &layouts[i];
    uint16_t total_length = current_group->uniforms.length +
                            current_group->textures.length +
                            current_group->samplers.length;
    uint16_t length = 0;

#ifdef VERBOSE_BINDING_PHASE
    VERBOSE_PRINT("    └ Bindgroup %d\n\t\t└ Uniforms: %lu\n\t\t└ Textures: "
                  "%lu\n\t\t└ Samplers: %lu\n",
                  current_group->index, current_group->uniforms.length,
                  current_group->textures.length,
                  current_group->samplers.length);
#endif

    WGPUBindGroupEntry *converted_entries =
        (WGPUBindGroupEntry *)malloc(total_length * sizeof(WGPUBindGroupEntry));

    // bind uniforms
    shader_bind_uniforms(shader, current_group, converted_entries, &length);
    // bind textures
    shader_bind_textures(shader, current_group, converted_entries, &length);
    // bind samplers
    shader_bind_samplers(shader, current_group, converted_entries, &length);

    // cache bind group
    current_group->bind_group = wgpuDeviceCreateBindGroup(
        *shader->device, &(WGPUBindGroupDescriptor){
                             .layout = wgpuRenderPipelineGetBindGroupLayout(
                                 shader->pipeline.handle, current_group->index),
                             .entryCount = total_length,
                             .entries = converted_entries,
                         });

    // release layouts
    wgpuBindGroupLayoutRelease(*current_layout);
    free(converted_entries);
  }
}
