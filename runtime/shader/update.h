#ifndef _SHADER_UPDATE_H_
#define _SHADER_UPDATE_H_

#include "../runtime/texture/texture.h"
#include "bindgroup.h"
#include "core.h"
#include "webgpu/webgpu.h"

typedef enum {
  ShaderBufferLifetime_Keep,
  ShaderBufferLifetime_Release,
} ShaderBufferLifetime;

typedef struct {
  TextureResolution width;
  TextureResolution height;
  TextureChannel channels;
  unsigned char *data;
  size_t size;
  WGPUTextureViewDimension dimension;
  WGPUTextureFormat format;
} ShaderUpdateTexture;

void shader_update_texture(Shader *, const bind_group_index, const bind_index,
                           const ShaderUpdateTexture *);

void shader_update_texture_view(Shader *, const bind_group_index,
                                const bind_index, WGPUTextureView,
                                WGPUTextureFormat);

void shader_update_uniform_data(Shader *, const bind_group_index,
                                const bind_index, void *);

void shader_update_uniform_buffer(Shader *, const bind_group_index,
                                  const bind_index, WGPUBuffer, const size_t,
                                  const ShaderBufferLifetime);

void shader_update_uniform_callback(Shader *, const bind_group_index,
                                    const bind_index,
                                    const ShaderUniformUpdate *);

void shader_update_sampler(Shader *, const bind_group_index, const bind_index,
                           const WGPUSamplerDescriptor *);

#endif
