#ifndef _SHADER_BIND_H_
#define _SHADER_BIND_H_
#include "core.h"

void shader_bind_group_init(Shader *);
void shader_bind_group_clear(Shader *);

void shader_bind_uniforms(Shader *, ShaderBindGroup *, WGPUBindGroupEntry *,
                          uint16_t *);
void shader_bind_textures(Shader *, ShaderBindGroup *, WGPUBindGroupEntry *,
                          uint16_t *);
void shader_bind_samplers(Shader *, ShaderBindGroup *, WGPUBindGroupEntry *,
                          uint16_t *);

ShaderBindGroup *shader_get_bind_group(Shader *, size_t);
bool shader_validate_binding(Shader *);
#endif
 
