#include "utils.h"
#include "utils/system.h"
#include <stdint.h>
/**
   Returns the sum of group uniforms, textures and samplers.
   Useful when creating each bind groups layout where the number of total
   entries is required.
 */
uint16_t shader_bind_group_entries_count(const ShaderBindGroup *group) {

  return group->uniforms.length + group->textures.length +
         group->samplers.length;

}

/**
   Check if the shader has every requirements before binding groups
   Also checks if the bind groups array isn't already at full capacity
 */

// TODO add more validation by uniforms type (UNIFORM/ TEX/ SAMPLER...) check
// if it doesn't overflow with max accepted length
bool shader_validate_binding(Shader *shader) {

  if (shader->device == NULL || shader->queue == NULL) {
    VERBOSE_ERROR("Shader has no device or queue.");
    return SHADER_BIND_UNVALID;
  }

  if (shader->bind_groups.length >= SHADER_MAX_BIND_GROUP) {
    VERBOSE_ERROR("Bind group list at full capacity.");
    return SHADER_BIND_UNVALID;
  }

  return SHADER_BIND_VALID;
}
