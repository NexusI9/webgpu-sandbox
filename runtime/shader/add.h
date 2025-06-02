#ifndef _SHADER_ADD_H_
#define _SHADER_ADD_H_
#include "core.h"
#include "bind.h"

void shader_add_uniform(Shader *, const ShaderCreateUniformDescriptor *);
void shader_add_texture(Shader *, const ShaderCreateTextureDescriptor *);
void shader_add_texture_view(Shader *,
                             const ShaderCreateTextureViewDescriptor *);
void shader_add_sampler(Shader *, const ShaderCreateSamplerDescriptor *);


#endif
