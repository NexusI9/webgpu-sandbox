#include "shader.h"
#include "../backend/generator.h"
#include "../utils/file.h"
#include "webgpu/webgpu.h"
#include <stdio.h>

shader shader_create(const ShaderCreateDescriptor *sd) {
  shader shader;

  // store shader string in memory
  store_file(&shader.source, sd->path);

  // compile shader module
  shader.module = create_shader(sd->device, shader.source, sd->label);

  return shader;
}

WGPUBindGroup shader_add_bind_group(shader *shader,
                                    const ShaderBindGroupDescriptor *bd) {
  WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(
      *bd->device, &(WGPUBindGroupDescriptor){
                       .layout = wgpuRenderPipelineGetBindGroupLayout(
                           *bd->pipeline, bd->group_index),
                       .entryCount = bd->entry_count,
                       // bind group entry
                       .entries = bd->entries,
                   });

  if (shader->bind_group_list.length < SHADER_MAX_BIND_GROUP) {
    shader->bind_group_list.items[shader->bind_group_list.length++] =
        bind_group;
  } else {
    perror("Bind group list at full capacity");
  }

  return bind_group;
}
