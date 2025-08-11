#include "build.h"
#include "bindgroup.h"
#include "core.h"
#include "layout.h"
#include "update.h"
#include "utils.h"
#include "webgpu/webgpu.h"

#include "../utils/system.h"

static inline void shader_build_pipeline(Shader *, WGPUBindGroupLayout *);

/**
   Build pipeline based on previously set bind groups.

 */
void shader_build(Shader *shader) {

  // clear pipeline if existing
#ifdef VERBOSE_BUILDING_PHASE
  VERBOSE_PRINT("\t└ Building Shader: %s", shader->name);
#endif

  // build bind group entries for each individual group index

  // Create Shader GPUBindGroup for each bindgroups
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *group = &shader->bind_groups.entries[i];

    // check if bind group is not already built
    // necessary cause in the wireframe mode we basically already built the
    // wireframe shader a first time for the boundbox but then a second time for
    // the wireframe topology, so we need to make sure it's not already built.
    // If it build a second time it actually accumulate new entries to the bind
    // group (goes from 3 to 6 entries) which lead to an error since the pieline
    // expect 3 entries.
    if (group->bind_group != NULL)
      continue;

#ifdef VERBOSE_BINDING_PHASE
    VERBOSE_PRINT("\t\t└ Bingroup: %d", i);
#endif
    shader_bind_group_build(group, i, shader->device,
                            &shader->pipeline->handle);
  }

  // TODO: properly release pipeline when deleting mesh
  // shader_pipeline_release_layout(shader);
  // free(bindgroup_layouts);
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

  // I. Create layout
  WGPUBindGroupLayout *bindgroup_layouts = shader_layout_build(shader);

  // II. Apply layout to pipeline
  WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
      shader->device, &(WGPUPipelineLayoutDescriptor){
                          // total bind groups count
                          .bindGroupLayoutCount = shader->bind_groups.length,
                          .bindGroupLayouts = layout,
                          .label = shader->name,
                      });

  // create pipeline
  // pipeline_build(shader->pipeline, &pipeline_layout);

  // release shader module after building pipeline
  shader_module_release(shader);
}
