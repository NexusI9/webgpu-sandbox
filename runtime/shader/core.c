#include "core.h"

#include "../backend/buffer.h"
#include "../utils/file.h"
#include "../utils/system.h"
#include "string.h"

/*

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

static void shader_set_vertex_layout(Shader *);

void shader_create(Shader *shader, const ShaderCreateDescriptor *sd) {

  // set name
  shader->name = strdup(sd->name);
  VERBOSE_PRINT("Creating shader: %s\n", shader->name);

  // store shader string in memory
  store_file(&shader->source, sd->path);

  // compile shader module intro GPU device
  buffer_create_shader(&shader->module, sd->device, shader->source, sd->label);
  shader->device = sd->device;
  shader->queue = sd->queue;

  // define bind groups length
  shader->bind_groups.length = 0;

  // set vertex layout
  shader_set_vertex_layout(shader);

  // init pipeline
  pipeline_create(&shader->pipeline,
                  &(PipelineCreateDescriptor){
                      .vertex_layout = &shader->vertex.layout,
                      .device = shader->device,
                      .module = &shader->module,
                  });
}

void shader_destroy(Shader *shader) {

  // clearing module
  wgpuShaderModuleRelease(shader->module);

  // clearing name
  free(shader->name);
  shader->name = NULL;

  // clearing pipeline
  if (shader->pipeline.handle)
    pipeline_destroy(shader_pipeline(shader));

  // clearing bind groups
  shader_bind_group_clear(shader);
}

/**
   Define standard vertex layout to be used in pipeline
   1. Position (vec3)
   2. Normals (vec3)
   3. Color (vec3)
   4. Texture Coordinate (vec2)
 */
void shader_set_vertex_layout(Shader *shader) {

  // set x,y,z
  shader->vertex.attribute[0] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 0,
      .shaderLocation = 0,
  };

  // set normals
  shader->vertex.attribute[1] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 3 * sizeof(float),
      .shaderLocation = 1,
  };

  // set r,g,b
  shader->vertex.attribute[2] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 6 * sizeof(float),
      .shaderLocation = 2,
  };

  // set u,v
  shader->vertex.attribute[3] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x2,
      .offset = 9 * sizeof(float),
      .shaderLocation = 3,
  };

  // define layout from attributes above
  shader->vertex.layout = (WGPUVertexBufferLayout){
      .arrayStride = VERTEX_STRIDE * sizeof(float),
      .attributeCount = 4,
      .attributes = shader->vertex.attribute,
  };
}

void shader_pipeline_release_layout(Shader *shader) {
  // Release pipeline
  wgpuPipelineLayoutRelease(shader->pipeline.layout);
}

/**
   Update method called as such: scene update => mesh update => shader update
 */
void shader_draw(Shader *shader, WGPURenderPassEncoder *render_pass,
                 const Camera *camera, const Viewport *viewport) {

  if (shader->pipeline.handle == NULL)
    return perror("Shader pipeline not defined for shader, skip drawing");

  // bind pipeline to render
  wgpuRenderPassEncoderSetPipeline(*render_pass, shader->pipeline.handle);

  // update bind group (uniforms, projection/view matrix...)
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_bind_group = &shader->bind_groups.entries[i];

    // update bindgroup uniforms data
    shader_uniform_update(current_bind_group, shader->queue);

    // link bind group
    wgpuRenderPassEncoderSetBindGroup(*render_pass, i,
                                      current_bind_group->bind_group, 0, NULL);
  }
}

void shader_module_release(Shader *shader) {
  // releasing shader module before drawing
  // invoked when adding the shader to the mesh (mesh_create)
  wgpuShaderModuleRelease(shader->module);
}

Pipeline *shader_pipeline(Shader *shader) { return &shader->pipeline; }

/**
   Access all uniforms from a bind group and check if it requires any update.
   If the trigger returns true, then it update the gpu buffer with the new data
   output from the callback
 */
void shader_uniform_update(ShaderBindGroup *group, const WGPUQueue *queue) {

  // update bindgroup entries (callback)
  for (int j = 0; j < group->uniforms.length; j++) {

    ShaderBindGroupUniformEntry *current_entry = &group->uniforms.entries[j];

    ShaderUniformUpdate *uniform_update = &current_entry->update;
    // TODO: separate dynamic (callback) from static (non callback) shader
    // in two arrays so no last minute decision

    // if no trigger (no gatekeep) or if trigger is true, then rewrite uniform
    // with callback
    // TODO: no update if out of frustrum ?
    if (uniform_update->callback &&
        (!uniform_update->trigger ||
         uniform_update->trigger(uniform_update->data, current_entry->data))) {

      // update uniform data
      uniform_update->callback(uniform_update->data, current_entry->data);

      // rewrite uniform to GPU
      wgpuQueueWriteBuffer(*queue, current_entry->buffer, 0,
                           current_entry->data, current_entry->size);
    }
  }
}
