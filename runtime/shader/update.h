#ifndef _SHADER_UPDATE_H_
#define _SHADER_UPDATE_H_

#include "../runtime/texture/texture.h"
#include "bindgroup.h"
#include "core.h"
#include "webgpu/webgpu.h"

typedef struct {
  TextureResolution width;
  TextureResolution height;
  TextureChannel channels;
  unsigned char *data;
  size_t size;
  WGPUTextureViewDimension dimension;
  WGPUTextureFormat format;
} ShaderUpdateTexture;

void shader_update_texture(Shader *, bind_group_index, bind_index,
                           const ShaderUpdateTexture *);

void shader_update_texture_view(Shader *, bind_group_index, bind_index,
                                WGPUTextureView, WGPUTextureFormat);

void shader_update_uniform(Shader *, bind_group_index, bind_index, void *);

void shader_update_uniform_callback(Shader *, bind_group_index, bind_index,
                                    const ShaderUniformUpdate *);

void shader_update_sampler(Shader *, bind_group_index, bind_index,
                           const WGPUSamplerDescriptor *);

#endif
