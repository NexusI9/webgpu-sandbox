#ifndef _SHADER_UPDATE_H_
#define _SHADER_UPDATE_H_

#include <stddef.h>
#include <stdint.h>

#include "../runtime/texture/texture.h"
#include "bindgroup.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include "../runtime/texture/core.h"

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

ShaderBindGroupTextureEntry *shader_update_texture(Shader *,
                                                   const bind_group_index,
                                                   const bind_index,
                                                   const ShaderUpdateTexture *);

ShaderBindGroupTextureEntry *
shader_update_texture_view(Shader *, const bind_group_index, const bind_index,
                           WGPUTextureView, WGPUTextureFormat);

ShaderBindGroupUniformEntry *shader_update_uniform_data(Shader *,
                                                        const bind_group_index,
                                                        const bind_index,
                                                        void *);

ShaderBindGroupUniformEntry *
shader_update_uniform_buffer(Shader *, const bind_group_index, const bind_index,
                             WGPUBuffer, const size_t,
                             const ShaderBufferLifetime);

ShaderBindGroupUniformEntry *
shader_update_uniform_callback(Shader *, const bind_group_index,
                               const bind_index, const ShaderUniformUpdate *);

ShaderBindGroupSamplerEntry *
shader_update_sampler(Shader *, const bind_group_index, const bind_index,
                      const WGPUSamplerDescriptor *);

ShaderBindGroup *shader_update_bind_group_offset(Shader *,
                                                 const bind_group_index,
                                                 const uint8_t, const size_t);
#endif
