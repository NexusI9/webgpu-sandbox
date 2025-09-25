#ifndef _PIPELINE_COMPUTE_H_
#define _PIPELINE_COMPUTE_H_

#include <stddef.h>
#include <stdint.h>

#include "runtime/geometry/vertex/core.h"
#include "runtime/geometry/vertex/vertex.h"
#include "webgpu/webgpu.h"

typedef struct {

} ComputePipelineBinding;

typedef struct {
  const char *label;

  // WGSL file to load module
  const char *shader_path;
  const WGPUBindGroupLayoutDescriptor *bind_groups[4];

  // Shader bind groups layout
  size_t bind_groups_count;

  ComputePipelineBinding bindings;

} ComputePipelineStateObject;

typedef struct {

  const char *label;
  const char *path;

  WGPUShaderModule module;

  // layout
  WGPUPipelineLayout layout;
  WGPUComputePipeline handle;

  // initial layout
  const ComputePipelineStateObject *shader_pso;

  // pipeline bindings
  ComputePipelineBinding bindings;

} ComputePipeline;

typedef struct {
  const char *label;
  const char *path;
  const ComputePipelineStateObject *pso;
  
} ComputePipelineCreateDescriptor;

void compute_pipeline_create(ComputePipeline *,
                             const ComputePipelineCreateDescriptor *);

void compute_pipeline_build(ComputePipeline *, const WGPUPipelineLayout *);

void compute_pipeline_destroy(ComputePipeline *);

#endif
