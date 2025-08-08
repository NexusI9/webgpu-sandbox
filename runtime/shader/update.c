#include "update.h"
#include "../backend/buffer.h"
#include "../backend/renderer/scene/std_texture/std_texture.h"
#include "../utils/system.h"
#include "bindgroup.h"
#include "core.h"
#include "find.h"
#include "utils.h"
#include "webgpu/webgpu.h"

/*TODO: BATCH UPDATE : like add, take a bunch of entry and ONLY REBUILD at the
 * end of update*/
void shader_update_texture_view(Shader *shader, bind_group_index group_index,
                                bind_index index, WGPUTextureView view,
                                WGPUTextureFormat format) {

  ShaderBindGroup *bind_group = shader_find_bind_group(shader, group_index);
  ShaderBindGroupTextureEntry *bound_texture =
      shader_find_texture(shader, group_index, index);

  if (bound_texture != NULL) {

    // clear the previous texture view if it is NOT a std texture
    if (!is_std_texture_view(bound_texture->texture_view))
      wgpuTextureViewRelease(bound_texture->texture_view);

    // replace the value
    bound_texture->texture_view = view;
    bound_texture->format = format;

    // rebuild group
    shader_bind_group_refresh(bind_group, group_index, shader->device,
                              &shader_pipeline(shader)->handle);

  } else {
    VERBOSE_WARNING(
        "Could not find the bound texture view in group: %d, index: %d, make "
        "sure "
        "the shader is correctly initialised with all bounds (shader: %s)",
        group_index, index, shader->name);
  }
}

void shader_update_uniform(Shader *shader, bind_group_index group_index,
                           bind_index index, void *data) {

  ShaderBindGroup *bind_group = shader_find_bind_group(shader, group_index);
  ShaderBindGroupUniformEntry *bound_uniform =
      shader_find_uniform(shader, group_index, index);

  if (bound_uniform != NULL) {

    bound_uniform->data = data;

    wgpuQueueWriteBuffer(shader->queue, bound_uniform->buffer, 0,
                         bound_uniform->data, bound_uniform->size);

    // rebuild group
    shader_bind_group_refresh(bind_group, group_index, shader->device,
                              &shader_pipeline(shader)->handle);

  } else {
    VERBOSE_WARNING(
        "Could not find the bound texture in group: %d, index: %d, make sure "
        "the shader is correctly initialised with all bounds (shader: %s)",
        group_index, index, shader->name);
  }
}

void shader_update_sampler(Shader *shader, bind_group_index group_index,
                           bind_index index,
                           const WGPUSamplerDescriptor *sampler) {

  ShaderBindGroup *bind_group = shader_find_bind_group(shader, group_index);
  ShaderBindGroupSamplerEntry *bound_sampler =
      shader_find_sampler(shader, group_index, index);

  // WGPUTextureView previous_texture_view = bound_texture->texture_view;

  if (bound_sampler != NULL) {

    wgpuSamplerRelease(bound_sampler->sampler);

    // replace the value
    bound_sampler->compare = sampler->compare;
    bound_sampler->addressModeU = sampler->addressModeU;
    bound_sampler->addressModeV = sampler->addressModeV;
    bound_sampler->addressModeW = sampler->addressModeW;
    bound_sampler->minFilter = sampler->minFilter;
    bound_sampler->magFilter = sampler->magFilter;

    bound_sampler->sampler = wgpuDeviceCreateSampler(shader->device, sampler);

    // rebuild group
    shader_bind_group_refresh(bind_group, group_index, shader->device,
                              &shader_pipeline(shader)->handle);

  } else {
    VERBOSE_WARNING(
        "Could not find the bound sampler in group: %d, index: %d, make sure "
        "the shader is correctly initialised with all bounds (shader: %s)",
        group_index, index, shader->name);
  }
}

void shader_update_texture(Shader *shader, bind_group_index group_index,
                           bind_index index,
                           const ShaderUpdateTexture *texture) {

  ShaderBindGroup *bind_group = shader_find_bind_group(shader, group_index);
  ShaderBindGroupTextureEntry *bound_texture =
      shader_find_texture(shader, group_index, index);

  if (bound_texture != NULL) {

    // generate texture + texture view from data & size
    WGPUTextureView new_view;
    buffer_create_texture(&new_view,
                          &(CreateTextureDescriptor){
                              .width = texture->width,
                              .height = texture->height,
                              .data = texture->data,
                              .size = texture->size,
                              .device = shader->device,
                              .queue = shader->queue,
                              .format = texture->format,
                              .channels = texture->channels,
                          },
                          BufferTextureMemory_Free);

    shader_update_texture_view(shader, group_index, index, new_view,
                               bound_texture->format);

  } else {
    VERBOSE_WARNING(
        "Could not find the bound texture in group: %d, index: %d, make sure "
        "the shader is correctly initialised with all bounds (shader: %s)",
        group_index, index, shader->name);
  }
}
