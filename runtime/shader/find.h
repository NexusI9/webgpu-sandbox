#ifndef _SHADER_FIND_H_
#define _SHADER_FIND_H_

#include "bindgroup.h"
#include "core.h"

ShaderBindGroup *shader_find_bind_group(Shader *, bind_group_index);
ShaderBindGroupUniformEntry *shader_find_uniform(Shader *, bind_group_index,
                                                 bind_index);
ShaderBindGroupTextureEntry *shader_find_texture(Shader *, bind_group_index,
                                                 bind_index);
ShaderBindGroupSamplerEntry *shader_find_sampler(Shader *, bind_group_index,
                                                 bind_index);

#endif
