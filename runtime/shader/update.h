#ifndef _SHADER_UPDATE_H_
#define _SHADER_UPDATE_H_

#include "bind.h"
#include "core.h"
#include "webgpu/webgpu.h"

void shader_update_texture(Shader *, bind_group_index, WGPUTextureView *,
                           bind_index);

void shader_update_uniform(Shader *, bind_group_index, void *, size_t,
                           bind_index);

void shader_update_sampler(Shader *, bind_group_index, WGPUSampler *,
                           bind_index);


#endif
