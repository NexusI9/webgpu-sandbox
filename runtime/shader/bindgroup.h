#ifndef _SHADER_BIND_H_
#define _SHADER_BIND_H_
#include "core.h"
#include <stddef.h>
#include <stdint.h>

typedef uint16_t bind_index;
typedef uint8_t bind_group_index;

typedef struct {
  const WGPUDevice device;
  const WGPURenderPipeline *pipeline_handle;
  bind_group_index group_index;
  uint16_t entryCount;
  WGPUBindGroupEntry *entries;
} ShaderBindGroupRealize;

/**
   Below binding functions are mostly used internally within the shader building
   process. They work cojointly with "layout" and "build"
 */
void shader_bind_group_create(Shader *, bind_group_index);
void shader_bind_group_clear(Shader *);

void shader_bind_group_create_from_layout(Shader *,
                                          const ShaderPipelineStateObject *);

WGPUBindGroupEntry *shader_bind_group_convert(ShaderBindGroup *);
void shader_bind_group_build(ShaderBindGroup *, bind_group_index,
                             const WGPUDevice, const WGPURenderPipeline *);
void shader_bind_group_release(ShaderBindGroup *);
void shader_bind_group_refresh(ShaderBindGroup *, bind_group_index,
                               const WGPUDevice, const WGPURenderPipeline *);

ShaderBindGroup *shader_get_bind_group(Shader *, bind_group_index);

#endif

