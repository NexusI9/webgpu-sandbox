#ifndef _SHADER_BUILD_H_
#define _SHADER_BUILD_H_

#include "core.h"

void shader_build_bind(Shader *, WGPUBindGroupLayout *);
void shader_build(Shader *);
WGPUBindGroupLayout *shader_build_layout(Shader *);
void shader_build_bind(Shader *, WGPUBindGroupLayout *);
void shader_build_pipeline(Shader *, WGPUBindGroupLayout *);

#endif
