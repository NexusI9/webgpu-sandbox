#include "core.h"
#include "../backend/buffer.h"
#include "../utils/file.h"
#include "build.h"
#include "string.h"

#include "../utils/system.h"
#include <stdint.h>

/*

  !! DEPRECATED NEED UDPATE !!
  OVERALL BUILDING PROCESS:
  1. First build layouts for uniforms, textures + samplers
  2. Build pipeline based on those layouts
  3. Actually bind the buffer/textures view and samplers

                  .------------------.
                  |   Add Buffer     |
                  |------------------|
                  | Add Uniforms     |
                  | Add Textures     |
                  | Add Samplers     |
                  '------------------'
                           ||
                  .------------------.
                  |   Build Layout   |
                  |------------------|
                  | Layout Uniforms  |
                  | Layout Textures  |
                  | Layout Samplers  |
                  '------------------'
                           ||
                  .------------------.
                  |  Build Pipeline  |
                  '------------------'
                           ||
                  .------------------.
                  |       Bind       |
                  |------------------|
                  | Bind Uniforms    |
                  | Bind Textures    |
                  | Bind Samplers    |
                  '------------------'


 */

void shader_create(Shader *shader, const ShaderCreateDescriptor *sd) {

  // set name
  shader->name = strdup(sd->name);

#ifdef VERBOSE_CREATING_PHASE
  VERBOSE_SHADER_CREATE("%s", shader->name);
#endif

  shader->device = sd->device;
  shader->queue = sd->queue;
  shader->pipeline = sd->pipeline;

  // define bind groups length
  shader->bind_groups.length = 0;

  // generate empty bindgroups based on pipeline layout (CPU Side)
  shader_bind_group_create_from_layout(shader,
                                       shader->pipeline->shader_pso);

  // create gpu bindgroups from generated layout (GPU side)
  shader_build(shader);
}

void shader_destroy(Shader *shader) {

  // clearing name
  free(shader->name);
  shader->name = NULL;

  // clearing bind groups
  shader_bind_group_clear(shader);
}

/**
   Update method called as such: scene update => mesh update => shader update
 */
void shader_draw(Shader *shader, WGPURenderPassEncoder render_pass) {
  
  // bind pipeline to render
  wgpuRenderPassEncoderSetPipeline(render_pass, shader->pipeline->handle);

  ShaderBindGroupList *dynamic_list = &shader->bind_groups;

  for (int i = 0; i < dynamic_list->length; i++) {

    ShaderBindGroup *current_bind_group = &dynamic_list->entries[i];

    // update bindgroup uniforms data
    shader_uniform_update(current_bind_group, shader->queue);

    // link bind group
    wgpuRenderPassEncoderSetBindGroup(render_pass, i,
                                      current_bind_group->bind_group, 0, NULL);
  }
}

void shader_module_release(Shader *shader) {
  // releasing shader module before drawing
  // invoked when adding the shader to the mesh (mesh_create)
  wgpuShaderModuleRelease(shader->pipeline->module);
}

const Pipeline *shader_pipeline(Shader *shader) { return shader->pipeline; }

/**
   Access all uniforms from a bind group and check if it requires any update.
   If the trigger returns true, then it update the gpu buffer with the new data
   output from the callback
 */
void shader_uniform_update(ShaderBindGroup *group, const WGPUQueue queue) {

  // update bindgroup entries (callback)
  ShaderBindGroupUniformsDynamics *dynamic_uniforms = &group->uniforms_dynamics;

  for (int j = 0; j < dynamic_uniforms->length; j++) {

    ShaderBindGroupUniformEntry *current_entry = dynamic_uniforms->entries[j];

    ShaderUniformUpdate *uniform_update = &current_entry->update;

    // if no trigger (no gatekeep) or if trigger is true, then rewrite uniform
    // with callback
    // TODO: no update if out of frustrum ?
    if (uniform_update->callback &&
        (!uniform_update->trigger ||
         uniform_update->trigger(uniform_update->data, current_entry->data))) {

      // update uniform data
      uniform_update->callback(uniform_update->data, current_entry->data);

      // rewrite uniform to GPU
      wgpuQueueWriteBuffer(queue, current_entry->buffer, 0, current_entry->data,
                           current_entry->size);
    }
  }
}
