#include "shader.h"
#include "../backend/generator.h"
#include "../utils/file.h"
#include "camera.h"
#include "webgpu/webgpu.h"
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

  // define pipeline
  // update_pipeline(&shader);

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
          .visibility = WGPUShaderStage_Vertex,
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

    // one pipeline correctly set, create bind group: link buffer to shader
    // pipeline
    WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(
        *shader->device, &(WGPUBindGroupDescriptor){
                             .layout = wgpuRenderPipelineGetBindGroupLayout(
                                 shader->pipeline, current_bind_group->index),
                             .entryCount = current_bind_group->entries.length,
                             .entries = current_bind_group->entries.items,
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
    wgpuRenderPassEncoderSetBindGroup(*render_pass, current_bind_group->index,
                                      current_bind_group->bind_group, 0, NULL);
  }
}

void shader_add_uniform(shader *shader,
                        const ShaderCreateUniformDescriptor *bd) {

  if (shader->device == NULL || shader->queue == NULL)
    perror("Shader has no device or queue"), exit(0);

  else if (shader->bind_groups.length < SHADER_MAX_BIND_GROUP) {

    // Steps:
    //   1. Increment bind group length
    //   2. update pipeline (update bind group entries)
    //   3. Create Buffer (GPU side)
    //     a. Allocate space in GPU
    //     b. Write data in buffer
    //   4. Store buffer reference into Uniform object (CPU side)

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
      shader->bind_groups.items[shader->bind_groups.length++].index =
          bd->group_index;

      // init new bind group entries legnth to 0
      shader->bind_groups.items[shader->bind_groups.length].entries.length = 0;
    }

    ShaderBindGroup *current_bind_group = &shader->bind_groups.items[index];

    // upload new uniforms buffer (CPU data => GPU) and store
    current_bind_group->buffer = create_buffer(&(CreateBufferDescriptor){
        .queue = shader->queue,
        .device = shader->device,
        .data = (void *)bd->data,
        .size = bd->size,
        .usage = WGPUBufferUsage_Uniform,
    });

    // combine argument entries with uniform buffer
    for (i = 0; i < bd->entry_count; i++) {

      // assign buffer to entries first
      bd->entries[i].buffer = current_bind_group->buffer;

      // finally transfer entries
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

  // bind camera
  CameraUniform cam_uni = camera_uniform(camera);

  WGPUBindGroupEntry entries[2] = {
      // camera
      {
          .binding = 0,
          .size = sizeof(CameraUniform),
          .offset = 0,
      },

      // viewport
      {
          .binding = 1,
          .size = sizeof(mat4),
          .offset = 0,
      },
  };

  shader_add_uniform(shader,
                     &(ShaderCreateUniformDescriptor){
                         .group_index = group_index,
                         .entry_count = 2,
                         .data = (void *[]){&cam_uni, &viewport->projection},
                         .size = sizeof(CameraUniform) + sizeof(mat4),
                         .entries = entries,
                     });
}

void shader_release(shader *shader) {
  // releasing shader module before drawing
  wgpuShaderModuleRelease(shader->module);
}
