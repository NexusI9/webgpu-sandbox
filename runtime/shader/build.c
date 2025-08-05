#include "build.h"
#include "bindgroup.h"
#include "layout.h"
#include "update.h"
#include "utils.h"
#include "webgpu/webgpu.h"

static inline void shader_build_pipeline(Shader *, WGPUBindGroupLayout *);

/**
   Build pipeline based on previously set bind groups.

 */
void shader_build(Shader *shader) {

  // clear pipeline if existing
#ifdef VERBOSE_BUILDING_PHASE
  VERBOSE_PRINT("  └ Building Shader: %s\n", shader->name);
#endif

  // build bind group entries for each individual group index

  // I. Create layout
  WGPUBindGroupLayout *bindgroup_layouts = shader_layout_build(shader);

  // II. Apply layout to pipeline
  shader_build_pipeline(shader, bindgroup_layouts);

  // III. Realize all bindgroups for GPU reference
  for (int i = 0; i < shader->bind_groups.length; i++)
    shader_bind_group_build(&shader->bind_groups.entries[i], i, shader->device,
                            &shader->pipeline.handle);

  // shader_module_release(shader);
  // TODO: properly release pipeline when deleting mesh
  // shader_pipeline_release_layout(shader);
  free(bindgroup_layouts);
}

/**
   II. Second phase of the shader build process.

   We previously created the bindgroup layout in the phase one according to the
   shader bindgroup entry.

   In this phase we "realize" the pipeline by providing it the dynamically
   created layout. This phase settle and define the actuall pipeline.

   During this phase we apply the pipeline its different attributes (primitive,
   fragment...)
 */
void shader_build_pipeline(Shader *shader, WGPUBindGroupLayout *layout) {

  WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
      shader->device, &(WGPUPipelineLayoutDescriptor){
                           // total bind groups count
                           .bindGroupLayoutCount = shader->bind_groups.length,
                           .bindGroupLayouts = layout,
                           .label = shader->name,
                       });

  // create pipeline
  pipeline_build(&shader->pipeline, &pipeline_layout);
}

/**
   Check if shader is already built by checking the pipeline handle (or layout).
   Prevent the shader "program" to be built twice while switching between
   drawing modes.
 */
bool shader_is_built(Shader *shader) { return shader->pipeline.handle != NULL; }
