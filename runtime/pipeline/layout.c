#include "layout.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stdlib.h>

#include "../utils/system.h"

/**
   Transforms bindgroups into pipeline layouts and returns the generated
   pipeline descriptor
 */
WGPUPipelineLayout shader_pipeline_state_object_create(
    const WGPUBindGroupLayoutDescriptor *const *bind_groups, const size_t count,
    const WGPUDevice device, WGPUBindGroupLayout *outLayout) {

  const size_t layout_size = sizeof(WGPUBindGroupLayout) * count;

  WGPUBindGroupLayout *layouts = malloc(layout_size);

  for (size_t i = 0; i < count; i++)
    layouts[i] = wgpuDeviceCreateBindGroupLayout(device, bind_groups[i]);

  if (outLayout != NULL)
    memcpy(outLayout, layouts, layout_size);

  WGPUPipelineLayout pipeline_layout =
      wgpuDeviceCreatePipelineLayout(device, &(WGPUPipelineLayoutDescriptor){
                                                 .bindGroupLayoutCount = count,
                                                 .bindGroupLayouts = layouts,
                                             });

  free(layouts);
  layouts = NULL;

  return pipeline_layout;
}
