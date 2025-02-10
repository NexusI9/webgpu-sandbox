#include "shader.h"
#include "../backend/generator.h"
#include "../utils/file.h"
#include "camera.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

static void set_vertex_layout(shader *);

shader shader_create(const ShaderCreateDescriptor *sd) {
  shader shader;

  // store shader string in memory
  store_file(&shader.source, sd->path);

  // compile shader module intro GPU device
  shader.module = create_shader(sd->device, shader.source, sd->label);
  shader.device = sd->device;
  shader.queue = sd->queue;

  // define bind groups length
  shader.bind_groups.length = 0;

  // set vertex layout
  set_vertex_layout(&shader);

  // set name
  shader.name = sd->name != NULL ? sd->name : "undefined";

  return shader;
}

// Define vertex layout to be used in pipeline
void set_vertex_layout(shader *shader) {

  // set x,y
  shader->vertex.attribute[0] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 0,
      .shaderLocation = 0,
  };

  // set r,g,b
  shader->vertex.attribute[1] = (WGPUVertexAttribute){
      .format = WGPUVertexFormat_Float32x3,
      .offset = 3 * sizeof(float),
      .shaderLocation = 1,
  };

  // define layout from attributes above
  shader->vertex.layout = (WGPUVertexBufferLayout){
      .arrayStride = 6 * sizeof(float),
      .attributeCount = 2,
      .attributes = shader->vertex.attribute,
  };
}

void shader_build(shader *shader) {

  // clear pipeline if existing
  if (shader->pipeline) {
    wgpuRenderPipelineRelease(shader->pipeline);
    shader->pipeline = NULL;
  }

  // build bind group entries for each individual group index
  WGPUBindGroupLayout bindgroup_layout[shader->bind_groups.length];

  // go through shader bind groups
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroupEntries *current_entries =
        &shader->bind_groups.items[i].entries;

    WGPUBindGroupLayoutEntry entries[current_entries->length];

    // go through each groups entries
    for (int j = 0; j < current_entries->length; j++) {
      entries[j] = (WGPUBindGroupLayoutEntry){
          // assign stored id
          .binding = current_entries->items[j].binding,
          // buffer binding layout
          .buffer = {.type = WGPUBufferBindingType_Uniform},
          // set visibility to vertex
          .visibility = shader->bind_groups.items[i].visibility,
      };
    }

    bindgroup_layout[i] = wgpuDeviceCreateBindGroupLayout(
        *shader->device, &(WGPUBindGroupLayoutDescriptor){
                             .entryCount = current_entries->length,
                             // bind group layout entry
                             .entries = entries,
                         });
  }

  WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
      *shader->device, &(WGPUPipelineLayoutDescriptor){
                           // total bind groups count
                           .bindGroupLayoutCount = shader->bind_groups.length,
                           .bindGroupLayouts = &bindgroup_layout[0],
                           .label = shader->name,
                       });

  // create pipeline
  shader->pipeline = wgpuDeviceCreateRenderPipeline(
      *shader->device,
      &(WGPURenderPipelineDescriptor){
          .layout = pipeline_layout,
          .label = "Shader",
          .vertex =
              {
                  .module = shader->module,
                  .entryPoint = "vs_main",
                  .bufferCount = 1,
                  .buffers = &shader->vertex.layout,
              },
          .primitive =
              {
                  .frontFace = WGPUFrontFace_CCW,
                  .cullMode = WGPUCullMode_None,
                  .topology = WGPUPrimitiveTopology_TriangleList,
                  .stripIndexFormat = WGPUIndexFormat_Undefined,
              },
          .fragment =
              &(WGPUFragmentState){
                  .module = shader->module,
                  .entryPoint = "fs_main",
                  .targetCount = 1,
                  // color target state
                  .targets =
                      &(WGPUColorTargetState){
                          .format = WGPUTextureFormat_BGRA8Unorm,
                          .writeMask = WGPUColorWriteMask_All,
                          // blend state
                          .blend =
                              &(WGPUBlendState){
                                  .color =
                                      {
                                          .operation = WGPUBlendOperation_Add,
                                          .srcFactor = WGPUBlendFactor_One,
                                          .dstFactor = WGPUBlendFactor_One,
                                      },
                                  .alpha =
                                      {
                                          .operation = WGPUBlendOperation_Add,
                                          .srcFactor = WGPUBlendFactor_One,
                                          .dstFactor = WGPUBlendFactor_One,
                                      },
                              },
                      },
              },
          .multisample =
              {
                  .count = 1,
                  .mask = 0xFFFFFFFF,
                  .alphaToCoverageEnabled = false,
              },
          .depthStencil = NULL,

      });

  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_bind_group = &shader->bind_groups.items[i];

    WGPUBindGroupEntry converted_entries[current_bind_group->entries.length];

    // map shader bind group entry to WGPU bind group entry
    // (basically the same just without data and callback attributes)
    for (int j = 0; j < current_bind_group->entries.length; j++) {
      ShaderBindGroupEntry *current_entry =
          &current_bind_group->entries.items[j];
      converted_entries[j].binding = current_entry->binding;
      converted_entries[j].buffer = current_entry->buffer;
      converted_entries[j].offset = current_entry->offset;
      converted_entries[j].size = current_entry->size;
    }

    // one pipeline correctly set, create bind group: link buffer to shader
    // pipeline
    WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(
        *shader->device, &(WGPUBindGroupDescriptor){
                             .layout = wgpuRenderPipelineGetBindGroupLayout(
                                 shader->pipeline, current_bind_group->index),
                             .entryCount = current_bind_group->entries.length,
                             .entries = converted_entries,
                         });

    current_bind_group->bind_group = bind_group;

    // release layouts
    wgpuBindGroupLayoutRelease(bindgroup_layout[i]);
  }

  // release pipeline
  wgpuPipelineLayoutRelease(pipeline_layout);
}

void shader_draw(shader *shader, WGPURenderPassEncoder *render_pass,
                 const camera *camera, const viewport *viewport) {

  // bind pipeline to render
  wgpuRenderPassEncoderSetPipeline(*render_pass, shader->pipeline);

  // update bind group (uniforms, projection/view matrix...)
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_bind_group = &shader->bind_groups.items[i];
    // update bindgroup entries (callback)
    for (int j = 0; j < current_bind_group->entries.length; j++) {
      ShaderBindGroupEntry *current_entry =
          &current_bind_group->entries.items[j];

      // TODO: separate dynamic (callback) from static (non callback) shader in
      // two arrays so no last minute decision
      // TODO 2: maybe add a "requires udpate" flag so more efficient update
      if (current_entry->update_callback) {
        current_entry->update_callback(current_entry->update_data,
                                       current_entry->data);
        wgpuQueueWriteBuffer(*shader->queue, current_entry->buffer, 0,
                             current_entry->data, current_entry->size);
      }
    }
    // link bind group

    wgpuRenderPassEncoderSetBindGroup(*render_pass, current_bind_group->index,
                                      current_bind_group->bind_group, 0, NULL);
  }
}

void shader_add_uniform(shader *shader,
                        const ShaderCreateUniformDescriptor *bd) {

  /*
    TODO: Currently we create a buffer per uniform so we don't need to worry
    about the alignment. However this approach is less optimal (since more call
    to WGPU) Need to create a way to combine uniforms data into 1 buffer and
    handle the alignment
   */

  if (shader->device == NULL || shader->queue == NULL)
    perror("Shader has no device or queue"), exit(0);

  else if (shader->bind_groups.length < SHADER_MAX_BIND_GROUP) {

    // Steps:
    //   - Increment bind group length
    //   - Create Buffer (GPU side)
    //     a. Allocate space in GPU
    //     b. Write data in buffer
    //   - Store buffer reference into Uniform object (CPU side)

    int in = 0, i = 0;
    size_t index = shader->bind_groups.length;
    for (i = 0; i < shader->bind_groups.length; i++) {
      // check if group index is already in shader bind group index
      if (bd->group_index == shader->bind_groups.items[i].index) {
        in = 1;    // true
        index = i; // override group
        break;
      }
    }

    // init new bind group
    if (in == 0) {

      // set index for further references
      index = shader->bind_groups.length;

      // Increment bind_groupd length (push new bind group)
      shader->bind_groups.items[shader->bind_groups.length].index =
          bd->group_index;

      // init new bind group entries legnth to 0
      shader->bind_groups.items[shader->bind_groups.length++].entries.length =
          0;
    }

    ShaderBindGroup *current_bind_group = &shader->bind_groups.items[index];
    current_bind_group->visibility = bd->visibility | WGPUShaderStage_Vertex;

    // combine argument entries with uniform buffer
    for (i = 0; i < bd->entry_count; i++) {

      ShaderBindGroupEntry *current_entry = &bd->entries[i];

      // assign buffer to entry
      bd->entries[i].buffer = create_buffer(&(CreateBufferDescriptor){
          .queue = shader->queue,
          .device = shader->device,
          .data = (void *)current_entry->data,
          .size = current_entry->size,
          .usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst,
      });

      // transfer entry to shader bind group list
      current_bind_group->entries.items[i] = bd->entries[i];

      // update length
      current_bind_group->entries.length++;
    }

  } else {
    perror("Bind group list at full capacity");
  }
}

void shader_bind_camera(shader *shader, camera *camera, viewport *viewport,
                        uint8_t group_index) {

  ShaderViewProjectionUniform proj_view_data;

  CameraUniform uCamera = camera_uniform(camera);
  ViewportUniform uViewport = viewport_uniform(viewport);

  ShaderBindGroupEntry entries[2] = {
      // camera
      {.binding = 0,
       .data = &uCamera,
       .size = sizeof(CameraUniform),
       .offset = 0,
       .update_callback = camera_update_uniform,
       .update_data = camera},

      // viewport
      {
          .binding = 1,
          .data = &uViewport,
          .size = sizeof(ViewportUniform),
          .offset = 0,
      },
  };

  shader_add_uniform(shader, &(ShaderCreateUniformDescriptor){
                                 .group_index = group_index,
                                 .entry_count = 2,
                                 .visibility = WGPUShaderStage_Vertex |
                                               WGPUShaderStage_Fragment,
                                 .entries = entries});
}

void shader_release(shader *shader) {
  // releasing shader module before drawing
  // invoked when adding the shader to the mesh (mesh_create)
  wgpuShaderModuleRelease(shader->module);
}
