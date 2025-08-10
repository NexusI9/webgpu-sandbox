#ifndef _SHADER_UPDATE_H_
#define _SHADER_UPDATE_H_

#include "bindgroup.h"
#include "core.h"
#include "webgpu/webgpu.h"

typedef struct {
  int width;
  int height;
  unsigned char *data;
  size_t size;
  uint8_t channels;
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
