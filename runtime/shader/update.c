#include "update.h"
#include "../utils/system.h"
#include "bind.h"
#include "core.h"
#include "find.h"
#include "utils.h"
#include "webgpu/webgpu.h"

static inline void shader_bind_group_release(ShaderBindGroup *);

void shader_update_texture(Shader *shader, bind_group_index group_index,
                           WGPUTextureView *texture, bind_index index) {

  ShaderBindGroup *bind_group = shader_find_bind_group(shader, group_index);
  ShaderBindGroupTextureEntry *bound_texture =
      shader_find_texture(shader, group_index, index);

  if (bound_texture != NULL) {

    // first release the current bind group
    // shader_bind_group_release(bind_group);

    // replace the value

    // rebuild the bind group

  } else {
    VERBOSE_WARNING(
        "Could not find the bound texture in group: %d, index: %d, make sure "
        "the shader is correctly initialised with all bounds (shader: %s)",
        group_index, index, shader->name);
  }
}

void shader_update_uniform(Shader *shader, bind_group_index group_index,
                           void *data, size_t size, bind_index index) {}

void shader_update_sampler(Shader *shader, bind_group_index group_indx,
                           WGPUSampler *sampler, bind_index index) {}

void shader_bind_group_release(ShaderBindGroup *shader_bind_group) {
  if (shader_bind_group->bind_group != NULL)
    wgpuBindGroupRelease(shader_bind_group->bind_group);
}

