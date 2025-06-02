#ifndef _LAYOUT_SHADER_H_
#define _LAYOUT_SHADER_H_

#include "core.h"

// layout methods
WGPUBindGroupLayout *shader_build_layout(Shader *);
void shader_layout_uniforms(Shader *, ShaderBindGroup *,
                            WGPUBindGroupLayoutEntry *, uint16_t *);
void shader_layout_textures(Shader *, ShaderBindGroup *,
                            WGPUBindGroupLayoutEntry *, uint16_t *);
void shader_layout_samplers(Shader *, ShaderBindGroup *,
                            WGPUBindGroupLayoutEntry *, uint16_t *);

#endif
