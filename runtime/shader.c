#include "shader.h"
#include "../backend/generator.h"
#include "../utils/file.h"
#include "camera.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include <stdio.h>

static void set_vertex_layout(shader *);
static void update_pipeline(shader *);

shader shader_create(const ShaderCreateDescriptor *sd) {
  shader shader;

  // store shader string in memory
  store_file(&shader.source, sd->path);

  // compile shader module intro GPU device
  shader.module = create_shader(sd->device, shader.source, sd->label);
  shader.device = sd->device;
  shader.queue = sd->queue;

  // define uniforms length
  shader.uniforms.length = 0;

  // define bind groups length
  shader.bind_groups.length = 0;

  // set vertex layout
  set_vertex_layout(&shader);

  // define pipeline
  update_pipeline(&shader);

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

void update_pipeline(shader *shader) {

  // clear pipeline if existing
  if (shader->pipeline) {
    wgpuRenderPipelineRelease(shader->pipeline);
    shader->pipeline = NULL;
  }

  // build bind group entries for each individual group index
  WGPUBindGroupLayoutEntry uniform_entries[shader->bind_groups.length];
  for (int i = 0; i < shader->bind_groups.length; i++) {
    uniform_entries[i] = (WGPUBindGroupLayoutEntry){
        // assign stored id
        .binding = shader->bind_groups.items[i],
        // buffer binding layout
        .buffer = {.type = WGPUBufferBindingType_Uniform},
        // set visibility to vertex
        .visibility = WGPUShaderStage_Vertex,
    };
  }

  WGPUBindGroupLayout bindgroup_layout = wgpuDeviceCreateBindGroupLayout(
      *shader->device,
      &(WGPUBindGroupLayoutDescriptor){
          .entryCount = shader->bind_groups.length,
          // bind group layout entry
          .entries =
              &(WGPUBindGroupLayoutEntry){
                  // assign stored id
                  .binding = 0,
                  // buffer binding layout
                  .buffer = {.type = WGPUBufferBindingType_Uniform},
                  // set visibility to vertex
                  .visibility = WGPUShaderStage_Vertex,
              },
      });

  WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
      *shader->device,
      &(WGPUPipelineLayoutDescriptor){
          // set layout to 0 or 1 depending if bind groups have been added
          // will throw warning if binGroupLAYOUTCount >
          // BinGroupLayouts.entries.length
          .bindGroupLayoutCount = (int)glm_min(shader->bind_groups.length, 1),
          .bindGroupLayouts = &bindgroup_layout,
      });

  // create pipeline
  printf("%p\n%d\n%lu\n", &shader->pipeline,
         (int)glm_min(shader->bind_groups.length, 1),
         shader->bind_groups.length);

  shader->pipeline = wgpuDeviceCreateRenderPipeline(
      *shader->device,
      &(WGPURenderPipelineDescriptor){
          .layout = pipeline_layout,
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

  // clear layouts
  wgpuBindGroupLayoutRelease(bindgroup_layout);
  wgpuPipelineLayoutRelease(pipeline_layout);
}

void shader_draw(const shader *shader, WGPURenderPassEncoder *render_pass,
                 const camera *camera, const viewport *viewport) {

  // bind pipeline to render
  wgpuRenderPassEncoderSetPipeline(*render_pass, shader->pipeline);

  // update bind groupd (uniforms, projection/view matrix...)
  for (int i = 0; i < shader->uniforms.length; i++) {
    wgpuRenderPassEncoderSetBindGroup(
        *render_pass, 0, shader->uniforms.items[i].bind_group, 0, 0);
  }
}

void shader_add_uniform(shader *shader,
                        const ShaderCreateUniformDescriptor *bd) {

  if (shader->device == NULL || shader->queue == NULL)
    perror("Shader has no device or queue"), exit(0);

  else if (shader->uniforms.length < SHADER_MAX_BIND_GROUP) {

    // Steps:
    //   1. Increment bind group length
    //   2. update pipeline (update bind group entries)
    //   3. Create Buffer (GPU side)
    //     a. Allocate space in GPU
    //     b. Write data in buffer
    //   4. Store buffer reference into Uniform object (CPU side)
    //   5. Bind buffer to shader bind group

    shader->uniforms.length += 1;

    // add bind group index in bind group array if not in it
    //(will be used for pipeline bind group layout)
    int in = 0, i = 0;
    for (i = 0; i < shader->bind_groups.length; i++) {
      // check if group index is already in shader bind group index
      if (bd->group_index == shader->bind_groups.items[i]) {
        in = 1;
        break;
      }
    }

    if (in == 0) {
      shader->bind_groups.items[shader->bind_groups.length++] = bd->group_index;
    }

    // update pipeline by allocating bind group space in pipeline
    update_pipeline(shader);

    // upload new uniforms buffer (CPU data => GPU)
    shader->uniforms.items[shader->uniforms.length - 1].buffer =
        create_buffer(&(CreateBufferDescriptor){
            .queue = shader->queue,
            .device = shader->device,
            .data = (void *)bd->data,
            .size = bd->size,
            .usage = WGPUBufferUsage_Uniform,
        });

    // combine argument entries with uniform buffer
    for (i = 0; i < bd->entry_count; i++) {
      bd->entries[i].buffer =
          shader->uniforms.items[shader->uniforms.length - 1].buffer;
    }

    // link buffer to shader pipeline
    WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(
        *shader->device, &(WGPUBindGroupDescriptor){
                             .layout = wgpuRenderPipelineGetBindGroupLayout(
                                 shader->pipeline, bd->group_index),
                             .entryCount = bd->entry_count,
                             .entries = bd->entries,
                         });

    shader->uniforms.items[shader->uniforms.length - 1].bind_group = bind_group;

  } else {
    perror("Bind group list at full capacity");
  }
}

void shader_bind_camera(shader *shader, camera *camera, viewport *viewport,
                        uint8_t group_index) {

  // bind camera
  CameraUniform cam_uni = camera_uniform(camera);
  shader_add_uniform(shader, &(ShaderCreateUniformDescriptor){
                                 .group_index = group_index,
                                 .entry_count = 1,
                                 .data = &cam_uni,
                                 .size = sizeof(CameraUniform),
                                 .entries =
                                     &(WGPUBindGroupEntry){
                                         .binding = 0,
                                         .size = sizeof(CameraUniform),
                                         .offset = 0,
                                     },
                             });

  // bind viewport
  shader_add_uniform(shader, &(ShaderCreateUniformDescriptor){
                                 .group_index = group_index,
                                 .entry_count = 1,
                                 .data = &viewport->projection,
                                 .size = sizeof(mat4),
                                 .entries =
                                     &(WGPUBindGroupEntry){
                                         .binding = 1,
                                         .size = sizeof(mat4),
                                         .offset = 0,
                                     },
                             });
}

void shader_release(shader *shader) {
  // releasing shader module before drawing
  wgpuShaderModuleRelease(shader->module);
}
