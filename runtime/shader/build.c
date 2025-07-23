#include "build.h"
#include "bind.h"
#include "layout.h"
#include "update.h"
#include "utils.h"
#include "webgpu/webgpu.h"

/**
   Build pipeline based on previously set bind groups
 */
void shader_build(Shader *shader) {

  // clear pipeline if existing
#ifdef VERBOSE_BUILDING_PHASE
  VERBOSE_PRINT("  └ Building Shader: %s\n", shader->name);
#endif

  // build bind group entries for each individual group index

  // I. Create layout
  WGPUBindGroupLayout *bindgroup_layouts = shader_build_layout(shader);

  // II. Apply layout to pipeline
  shader_build_pipeline(shader, bindgroup_layouts);

  // III. Create GPU bindgroups
  shader_build_bind(shader, bindgroup_layouts);

  // shader_module_release(shader);
  shader_pipeline_release_layout(shader);
  free(bindgroup_layouts);
}

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
WGPUBindGroupLayout *shader_build_layout(Shader *shader) {

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
        *shader->device, &(WGPUBindGroupLayoutDescriptor){
                             .entryCount = total_length,
                             .entries = layout_entries,
                         });

    free(layout_entries);
  }

  return layout_list;
}

/**
   II. Second phase of the shader build process
   We previously created the bindgroup layout in the phase one according to the
   shader bindgroup entry.

   In this phase we "realize" the pipeline by providing it the dynamically
   created layout. This phase settle and define the actuall pipeline.

   During this phase we apply the pipeline its different attributes (primitive,
   fragment...)
 */
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

/**
   III. Last phase of the shader build process.
   In the previous steps we:
   1. Organise the layouts.
   2. Apply the layouts in the pipeline.

   Howerver the pipeline only own a layout/ blueprint an not the content itself.
   Meaning we still have to upload the actual bindgroup content to use it in the
   shader. That's the purpose of this latest phase.

   Note that this binding process is used during the first instantiation of the
   mesh. It bascially traverse the shader bindgroups and layout all the provded
   uniforms/ textures and sampler at once.

   Overall flow:
   1. Traverse all uniforms/textures and samplers from all bind groups
   2. Convert entries (tex/smpl/unfrm) to generic "WGPUBindGroupEntry"
   3. Store those converted entries in an array or entry.
   4. Create bind group based on the converted entries.
 */
void shader_build_bind(Shader *shader, WGPUBindGroupLayout *layouts) {

  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_group = &shader->bind_groups.entries[i];
    WGPUBindGroupLayout *current_layout = &layouts[i];
    uint16_t total_length = shader_bind_group_entries_count(current_group);

#ifdef VERBOSE_BINDING_PHASE
    VERBOSE_PRINT("    └ Bindgroup %d\n\t\t└ Uniforms: %lu\n\t\t└ Textures: "
                  "%lu\n\t\t└ Samplers: %lu\n",
                  current_group->index, current_group->uniforms.length,
                  current_group->textures.length,
                  current_group->samplers.length);
#endif

    WGPUBindGroupEntry *converted_entries =
        shader_bind_group_convert(current_group);

    // realize bind group
    shader_bind_group_realize(&current_group->bind_group,
                              &(ShaderBindGroupRealize){
                                  .group_index = i,
                                  .device = shader->device,
                                  .entryCount = total_length,
                                  .entries = converted_entries,
                                  .pipeline_handle = &shader->pipeline.handle,
                              });

    // release layouts
    wgpuBindGroupLayoutRelease(*current_layout);
    free(converted_entries);
  }
}

/**
   Check if shader is already built by checking the pipeline handle (or layout).
   Prevent the shader "program" to be built twice while switching between
   drawing modes.
 */
bool shader_is_built(Shader *shader) { return shader->pipeline.handle != NULL; }
