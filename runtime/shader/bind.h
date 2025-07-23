#ifndef _SHADER_BIND_H_
#define _SHADER_BIND_H_
#include "core.h"
#include <stddef.h>
#include <stdint.h>

typedef uint16_t bind_index;
typedef uint8_t bind_group_index;

#define SHADER_BIND_VALID 0
#define SHADER_BIND_UNVALID 1


typedef struct {
  const WGPUDevice *device;
  WGPURenderPipeline *pipeline_handle;
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

WGPUBindGroupEntry *shader_bind_group_convert(ShaderBindGroup*);
void shader_bind_group_realize(WGPUBindGroup *, const ShaderBindGroupRealize *);

ShaderBindGroup *shader_get_bind_group(Shader *, bind_group_index);

bool shader_validate_binding(Shader *);
#endif
