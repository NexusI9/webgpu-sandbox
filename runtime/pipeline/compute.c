#include "compute.h"
#include "backend/buffer.h"
#include "backend/context.h"
#include "utils/file.h"
#include "webgpu/webgpu.h"

/**
  Initialize the default pipeline with a preset descriptor
 */
void compute_pipeline_create(ComputePipeline *pipeline,
                             const ComputePipelineCreateDescriptor *desc) {

  // Define core data
  pipeline->handle = NULL;
  pipeline->label = desc->label;
  pipeline->shader_pso = desc->pso;
  pipeline->bindings = pipeline->shader_pso->bindings;

  char *source; // shader source code

  // store shader string in memory
  store_file(&source, desc->path);

  // compile shader module intro GPU device
  buffer_create_shader(&pipeline->module, source, pipeline->label);
}

/**
   Release pipeline if exists and create i new one
 */
void compute_pipeline_build(ComputePipeline *pipeline,
                            const WGPUPipelineLayout *layout) {

  // update bind group layout
  pipeline->layout = *layout;

  if (pipeline->handle)
    compute_pipeline_destroy(pipeline);

  pipeline->handle = wgpuDeviceCreateComputePipeline(
      context_device(), &(WGPUComputePipelineDescriptor){
                            .label = pipeline->label,
                            .layout = pipeline->layout,
                            .compute =
                                (WGPUProgrammableStageDescriptor){
                                    .module = pipeline->module,
                                    .entryPoint = "main",
                                },
                        });
}

/**
   Release pipeline and set back the handle to null
 */
void compute_pipeline_destroy(ComputePipeline *pipeline) {

  // clearing module
  wgpuShaderModuleRelease(pipeline->module);

  wgpuShaderModuleRelease(pipeline->module);
  wgpuComputePipelineRelease(pipeline->handle);
  pipeline->handle = NULL;

  // DO NOT uncomment, it causes crashes,
  // probably cause the layout is still in use
  // wgpuPipelineLayoutRelease(pipeline->layout);
  pipeline->layout = NULL;
}
