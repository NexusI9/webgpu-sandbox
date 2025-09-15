#include "find.h"
#include "../utils/system.h"
#include "core.h"

/**
   Find a specific bind group in the shader.
   Can be useful to manipulate bind group content (i.e. replacing uniform/
   texture values)

   Since bind groups are fixed array we can simply point to the group index
   without linear search.
 */
ShaderBindGroup *shader_find_bind_group(Shader *shader,
                                        bind_group_index group_index) {

  return &shader->bind_groups.entries[group_index];
}

/**
   Find a uniform entry in the given bind group.
   Used to eventually get or replace it value before rebinding the shader
   bindgroups.
 */
ShaderBindGroupUniformEntry *shader_find_uniform(Shader *shader,
                                                 bind_group_index group_index,
                                                 bind_index index,
                                                 size_t *list_index) {

  ShaderBindGroup *bind_group = shader_find_bind_group(shader, group_index);

  if (bind_group == NULL) {
    VERBOSE_WARNING("Could not find the group index at %d (Shader: %s)",
                    group_index, shader->name);
    return NULL;
  }



  ShaderBindGroupUniforms *uniforms = &bind_group->uniforms;

  if (uniforms->entries == NULL) {
    VERBOSE_WARNING("Uniforms not initialized in bind group: %d (shader: %s)",
                    group_index, shader->name);
    return NULL;
  }
  
  for (size_t i = 0; i < uniforms->length; i++) {
    if (uniforms->entries[i].binding == index) {
      if (list_index)
        *list_index = i;
      return &uniforms->entries[i];
    }
  }

  return NULL;
}

ShaderBindGroupTextureEntry *shader_find_texture(Shader *shader,
                                                 bind_group_index group_index,
                                                 bind_index index,
                                                 size_t *list_index) {
  ShaderBindGroup *bind_group = shader_find_bind_group(shader, group_index);

  if (bind_group == NULL) {
    VERBOSE_WARNING("Could not find the group index at %d (shader: %s)",
                    group_index, shader->name);
    return NULL;
  }

  ShaderBindGroupTextures *textures = &bind_group->textures;

  if (textures->entries == NULL) {
    VERBOSE_WARNING("Textures not initialized in bind group: %d (shader: %s)",
                    group_index, shader->name);
    return NULL;
  }

  for (size_t i = 0; i < textures->length; i++)
    if (textures->entries[i].binding == index) {
      if (list_index)
        *list_index = i;
      return &textures->entries[i];
    }

  return NULL;
}

ShaderBindGroupSamplerEntry *shader_find_sampler(Shader *shader,
                                                 bind_group_index group_index,
                                                 bind_index index,
                                                 size_t *list_index) {
  ShaderBindGroup *bind_group = shader_find_bind_group(shader, group_index);

  if (bind_group == NULL) {
    VERBOSE_WARNING("Could not find the group index at %d (Shader: %s)",
                    group_index, shader->name);
    return NULL;
  }

  ShaderBindGroupSamplers *samplers = &bind_group->samplers;

  if (samplers->entries == NULL) {
    VERBOSE_WARNING("Samplers not initialized in bind group: %d (shader: %s)",
                    group_index, shader->name);
    return NULL;
  }

  for (size_t i = 0; i < samplers->length; i++)
    if (samplers->entries[i].binding == index) {
      if (list_index)
        *list_index = i;
      return &samplers->entries[i];
    }

  return NULL;
}
