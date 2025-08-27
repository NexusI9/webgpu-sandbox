#include "core.h"
#include "../backend/buffer.h"
#include "../utils/file.h"
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
  shader_bind_group_create_from_layout(shader, shader->pipeline->shader_pso);

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

    ShaderBindGroup *bind_group = &dynamic_list->entries[i];

    // update bindgroup uniforms data
    shader_uniform_update(bind_group, shader->queue);

    // link bind group
    wgpuRenderPassEncoderSetBindGroup(render_pass, i, bind_group->bind_group,
                                      bind_group->offset.count,
                                      bind_group->offset.entries);
    
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
      wgpuQueueWriteBuffer(queue, current_entry->buffer, current_entry->offset,
                           current_entry->data, current_entry->size);
    }
  }
}

/**
   Build pipeline based on previously set bind groups.

 */
void shader_build(Shader *shader) {

  // clear pipeline if existing
#ifdef VERBOSE_BINDING_PHASE
  VERBOSE_PRINT("\t\t└ Binding Shader: %s", shader->name);
#endif

  // build bind group entries for each individual group index

  // Create Shader GPUBindGroup for each bindgroups
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *group = &shader->bind_groups.entries[i];

    // check if bind group is not already built
    // necessary cause in the wireframe mode we basically already built the
    // wireframe shader a first time for the boundbox but then a second time for
    // the wireframe topology, so we need to make sure it's not already built.
    // If it build a second time it actually accumulate new entries to the bind
    // group (goes from 3 to 6 entries) which lead to an error since the pieline
    // expect 3 entries.
    if (group->bind_group != NULL)
      continue;

#ifdef VERBOSE_BINDING_PHASE
    VERBOSE_PRINT("\t\t\t└ Bingroup: %d", i);
#endif
    shader_bind_group_build(group, i, shader->device,
                            &shader->pipeline->handle);
  }

  // TODO: properly release pipeline when deleting mesh
  // shader_pipeline_release_layout(shader);
  // free(bindgroup_layouts);
}
