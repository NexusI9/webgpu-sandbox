#include "utils.h"

/**
   Returns the sum of group uniforms, textures and samplers.
   Useful when creating each bind groups layout where the number of total
   entries is required.
 */
uint16_t shader_bind_group_entries_count(const ShaderBindGroup *group) {

  return group->uniforms.length + group->textures.length +
         group->samplers.length;
}
