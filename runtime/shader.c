#include "shader.h"
#include "../backend/buffer.h"
#include "../resources/geometry/vertex.h"
#include "../utils/file.h"
#include "../utils/system.h"
#include "camera.h"
#include "pipeline.h"
#include "string.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

static void shader_set_vertex_layout(shader *);

// build related methods
static ShaderBindGroup *shader_get_bind_group(shader *, size_t);
static bool shader_validate_binding(shader *);
static void shader_build_pipeline(shader *, WGPUBindGroupLayout *);

// layout methods
static WGPUBindGroupLayout *shader_build_layout(shader *);
static void shader_layout_uniforms(shader *, ShaderBindGroup *,
                                   WGPUBindGroupLayoutEntry *, uint16_t *);
static void shader_layout_textures(shader *, ShaderBindGroup *,
                                   WGPUBindGroupLayoutEntry *, uint16_t *);
static void shader_layout_samplers(shader *, ShaderBindGroup *,
                                   WGPUBindGroupLayoutEntry *, uint16_t *);

// build methods
static void shader_build_bind(shader *, WGPUBindGroupLayout *);
static void shader_bind_uniforms(shader *, ShaderBindGroup *,
                                 WGPUBindGroupEntry *, uint16_t *);
static void shader_bind_textures(shader *, ShaderBindGroup *,
                                 WGPUBindGroupEntry *, uint16_t *);
static void shader_bind_samplers(shader *, ShaderBindGroup *,
                                 WGPUBindGroupEntry *, uint16_t *);

void shader_create(shader *shader, const ShaderCreateDescriptor *sd) {

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

void shader_destroy(shader *shader) {

  // clearing module
  wgpuShaderModuleRelease(shader->module);

  // clearing name
  free(shader->name);
  shader->name = NULL;

  // clearing pipeline
  if(shader->pipeline.handle)
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
void shader_set_vertex_layout(shader *shader) {

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

static void shader_pipeline_release_layout(shader *shader) {
  // Release pipeline
  wgpuPipelineLayoutRelease(shader->pipeline.layout);
}

/**
   Build pipeline based on previously set bind groups
 */
void shader_build(shader *shader) {

  // clear pipeline if existing
#ifdef VERBOSE_BUILDING_PHASE
  VERBOSE_PRINT("  └ Building Shader: %s\n", shader->name);
#endif

  // build bind group entries for each individual group index
  WGPUBindGroupLayout *bindgroup_layouts = shader_build_layout(shader);
  shader_build_pipeline(shader, bindgroup_layouts);
  shader_build_bind(shader, bindgroup_layouts);

  // shader_module_release(shader);
  shader_pipeline_release_layout(shader);
  free(bindgroup_layouts);
}

WGPUBindGroupLayout *shader_build_layout(shader *shader) {

  /*
    need to first define bind group layout before actually pushing values in it
    divide the uniforms type if different classes (uniforms/ textures/ sampler)
    as they require dedicated layouts.
    Layouts define in a higher level what the GPU expects in term of type and
    structure

    .----------.      +===========+     .---------.     .----------.
    |  SHADER  | ==> || PIPELINE || <== | LAYOUTS | <== | UNIFORMS |
    '----------'     +===========+      '---------'     '----------'
        GPU                                                 CPU

  */

  // need to use malloc cause of VLA (variable length array)
  WGPUBindGroupLayout *layout_list = (WGPUBindGroupLayout *)malloc(
      shader->bind_groups.length * sizeof(WGPUBindGroupLayout));

  // go through shader bind groups and combine entries
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_group = &shader->bind_groups.entries[i];
    WGPUBindGroupLayout *current_layout = &layout_list[i];

    // combine all bind group entries in one array
    uint16_t total_length = current_group->uniforms.length +
                            current_group->textures.length +
                            current_group->samplers.length;

    uint16_t length = 0;

    WGPUBindGroupLayoutEntry *layout_entries =
        (WGPUBindGroupLayoutEntry *)malloc(total_length *
                                           sizeof(WGPUBindGroupLayoutEntry));

    // layout uniforms
    shader_layout_uniforms(shader, current_group, layout_entries, &length);
    // layout textures
    shader_layout_textures(shader, current_group, layout_entries, &length);
    // layout samplers
    shader_layout_samplers(shader, current_group, layout_entries, &length);

    // create layout from previously populated entries array
    *current_layout = wgpuDeviceCreateBindGroupLayout(
        *shader->device, &(WGPUBindGroupLayoutDescriptor){
                             .entryCount = total_length,
                             .entries = layout_entries,
                         });

    free(layout_entries);
  }

  return layout_list;
}

void shader_layout_uniforms(shader *shader, ShaderBindGroup *bindgroup,
                            WGPUBindGroupLayoutEntry *entries,
                            uint16_t *length) {

  ShaderBindGroupUniforms *uniform_entries = &bindgroup->uniforms;

  // go through each entries
  for (int j = 0; j < uniform_entries->length; j++) {
    entries[(*length)++] = (WGPUBindGroupLayoutEntry){
        // assign stored binding index
        .binding = uniform_entries->entries[j].binding,
        // buffer binding layout
        .buffer = {.type = WGPUBufferBindingType_Uniform},
        // set visibility to vertex
        .visibility = bindgroup->visibility,
    };
  }
}

void shader_layout_textures(shader *shader, ShaderBindGroup *bindgroup,
                            WGPUBindGroupLayoutEntry *entries,
                            uint16_t *length) {

  ShaderBindGroupTextures *texture_entries = &bindgroup->textures;

  // go through each entries
  for (int j = 0; j < texture_entries->length; j++) {
    ShaderBindGroupTextureEntry *current_entry = &texture_entries->entries[j];
    entries[(*length)++] = (WGPUBindGroupLayoutEntry){
        .texture =
            {
                .sampleType = current_entry->sample_type,
                .viewDimension = current_entry->dimension,
            },
        .binding = current_entry->binding,
        .visibility = bindgroup->visibility,
    };
  }
}

void shader_layout_samplers(shader *shader, ShaderBindGroup *bindgroup,
                            WGPUBindGroupLayoutEntry *entries,
                            uint16_t *length) {

  ShaderBindGroupSamplers *sampler_entries = &bindgroup->samplers;

  // go through each entries
  for (int j = 0; j < sampler_entries->length; j++) {
    ShaderBindGroupSamplerEntry *current_entry = &sampler_entries->entries[j];
    entries[(*length)++] = (WGPUBindGroupLayoutEntry){
        .sampler = {.type = current_entry->type},
        .binding = sampler_entries->entries[j].binding,
        .visibility = bindgroup->visibility,
    };
  }
}

void shader_build_pipeline(shader *shader, WGPUBindGroupLayout *layout) {

  WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(
      *shader->device, &(WGPUPipelineLayoutDescriptor){
                           // total bind groups count
                           .bindGroupLayoutCount = shader->bind_groups.length,
                           .bindGroupLayouts = layout,
                           .label = shader->name,
                       });

  // create pipeline
  pipeline_build(&shader->pipeline, &pipeline_layout);
}

void shader_build_bind(shader *shader, WGPUBindGroupLayout *layouts) {

  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_group = &shader->bind_groups.entries[i];
    WGPUBindGroupLayout *current_layout = &layouts[i];
    uint16_t total_length = current_group->uniforms.length +
                            current_group->textures.length +
                            current_group->samplers.length;
    uint16_t length = 0;

#ifdef VERBOSE_BINDING_PHASE
    VERBOSE_PRINT("    └ Bindgroup %d\n\t\t└ Uniforms: %lu\n\t\t└ Textures: "
                  "%lu\n\t\t└ Samplers: %lu\n",
                  current_group->index, current_group->uniforms.length,
                  current_group->textures.length,
                  current_group->samplers.length);
#endif

    WGPUBindGroupEntry *converted_entries =
        (WGPUBindGroupEntry *)malloc(total_length * sizeof(WGPUBindGroupEntry));

    // bind uniforms
    shader_bind_uniforms(shader, current_group, converted_entries, &length);
    // bind textures
    shader_bind_textures(shader, current_group, converted_entries, &length);
    // bind samplers
    shader_bind_samplers(shader, current_group, converted_entries, &length);

    // cache bind group
    current_group->bind_group = wgpuDeviceCreateBindGroup(
        *shader->device, &(WGPUBindGroupDescriptor){
                             .layout = wgpuRenderPipelineGetBindGroupLayout(
                                 shader->pipeline.handle, current_group->index),
                             .entryCount = total_length,
                             .entries = converted_entries,
                         });

    // release layouts
    wgpuBindGroupLayoutRelease(*current_layout);
    free(converted_entries);
  }
}

void shader_bind_uniforms(shader *shader, ShaderBindGroup *bindgroup,
                          WGPUBindGroupEntry *entries, uint16_t *index) {

  // map shader bind group entry to WGPU bind group entry
  // (basically the same just without data and callback attributes)
  for (int j = 0; j < bindgroup->uniforms.length; j++) {
    ShaderBindGroupUniformEntry *current_entry =
        &bindgroup->uniforms.entries[j];
    entries[(*index)++] = (WGPUBindGroupEntry){
        .binding = current_entry->binding,
        .buffer = current_entry->buffer,
        .offset = current_entry->offset,
        .size = current_entry->size,
    };
  }
}

void shader_bind_textures(shader *shader, ShaderBindGroup *bindgroup,
                          WGPUBindGroupEntry *entries, uint16_t *index) {

  // map shader bind group entry to WGPU bind group entry
  // (basically the same just without data and callback attributes)
  for (int j = 0; j < bindgroup->textures.length; j++) {
    ShaderBindGroupTextureEntry *current_entry =
        &bindgroup->textures.entries[j];
    entries[(*index)++] = (WGPUBindGroupEntry){
        .binding = current_entry->binding,
        .textureView = current_entry->texture_view,
    };
  }
}

void shader_bind_samplers(shader *shader, ShaderBindGroup *bindgroup,
                          WGPUBindGroupEntry *entries, uint16_t *index) {

  for (int j = 0; j < bindgroup->samplers.length; j++) {
    ShaderBindGroupSamplerEntry *current_entry =
        &bindgroup->samplers.entries[j];
    entries[(*index)++] = (WGPUBindGroupEntry){
        .binding = current_entry->binding,
        .sampler = current_entry->sampler,
    };
  }
}

/**
   Update method called as such: scene update => mesh update => shader update
 */
void shader_draw(shader *shader, WGPURenderPassEncoder *render_pass,
                 const camera *camera, const viewport *viewport) {

  if (shader->pipeline.handle == NULL)
    return perror("Shader pipeline not defined for shader, skip drawing");

  // bind pipeline to render
  wgpuRenderPassEncoderSetPipeline(*render_pass, shader->pipeline.handle);

  // update bind group (uniforms, projection/view matrix...)
  for (int i = 0; i < shader->bind_groups.length; i++) {

    ShaderBindGroup *current_bind_group = &shader->bind_groups.entries[i];

    // update bindgroup entries (callback)
    for (int j = 0; j < current_bind_group->uniforms.length; j++) {

      ShaderBindGroupUniformEntry *current_entry =
          &current_bind_group->uniforms.entries[j];

      // TODO: separate dynamic (callback) from static (non callback) shader
      // in two arrays so no last minute decision
      // TODO 2: maybe add a "requires udpate" flag so more efficient update
      // !! issue here
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

/**
   Add uniform of type Default (vec3, float...) into the shader
 */
void shader_add_uniform(shader *shader,
                        const ShaderCreateUniformDescriptor *bd) {

  /*
    TODO: Currently we create a buffer per uniform so we don't need to worry
    about the alignment. However this approach is less optimal (since more
    call to GPU) Need to create a way to combine uniforms data into 1 buffer
    and handle the alignment
   */

  if (shader_validate_binding(shader)) {
    /*
      Steps:
        - Increment bind group length
        - Create Buffer (GPU side)
          a. Allocate space in GPU
          b. Write data in buffer
        - Store buffer reference into Uniform object (CPU side)
      */

    ShaderBindGroup *current_bind_group =
        shader_get_bind_group(shader, bd->group_index);

    current_bind_group->visibility = bd->visibility | WGPUShaderStage_Vertex;

    // combine argument entries with uniform buffer
    for (int i = 0; i < bd->entry_count; i++) {

      ShaderBindGroupUniformEntry *current_entry = &bd->entries[i];

      // assign buffer to entry
      buffer_create(
          &bd->entries[i].buffer,
          &(CreateBufferDescriptor){
              .queue = shader->queue,
              .device = shader->device,
              .data = (void *)current_entry->data,
              .size = current_entry->size,
              .usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst,
              .mappedAtCreation = false,
          });

      /*
        Need to dynamically allocate the uniform if it has a callback function,
        cause when we use its pointer during the shader draw process, it
        prevents conflicts if two uniform data have the same address.
        By this we ensure all data have different addresses and prevent
        overwriting conflicts.

        static alloc:
        [mesh_1] uCamera => 0xefd091
        [mesh_2] uCamera => 0xefd091

        dynamic alloc:
        [mesh_1] uCamera => 0xefd092
        [mesh_2] uCamera => 0x48fd23

      */

      if (current_entry->update_callback) {
        void *temp_data = current_entry->data;
        current_entry->data = malloc(current_entry->size);
        memcpy(current_entry->data, temp_data, current_entry->size);
      }

      // transfer entry to shader bind group list
      current_bind_group->uniforms
          .entries[current_bind_group->uniforms.length++] = *current_entry;
    }
  }
}

/**
   Add uniform of type Texture into the shader
   The function upload the texture to the buffer
   and automatically handles the texture view creation.

   This function is usefull in case one want to upload and bind
   a texture from "raw data" when reading a file from disk (stbi, gltf...)
 */
void shader_add_texture(shader *shader,
                        const ShaderCreateTextureDescriptor *desc) {

  // printf("inner shader %p\n", shader);
  if (shader_validate_binding(shader)) {
    ShaderBindGroup *current_bind_group =
        shader_get_bind_group(shader, desc->group_index);

    current_bind_group->visibility =
        desc->visibility | WGPUShaderStage_Fragment;

    for (int i = 0; i < desc->entry_count; i++) {

      if (current_bind_group->textures.length ==
          current_bind_group->textures.capacity) {
        VERBOSE_PRINT("Texture array reached maximum capacity\n");
        break;
      }

      ShaderBindGroupTextureEntry *current_entry = &desc->entries[i];
      // generate texture + texture view from data & size
      buffer_create_texture(&current_entry->texture_view,
                            &(CreateTextureDescriptor){
                                .width = current_entry->width,
                                .height = current_entry->height,
                                .data = current_entry->data,
                                .size = current_entry->size,
                                .device = shader->device,
                                .queue = shader->queue,
                                .format = current_entry->format,
                                .channels = current_entry->channels,
                            });

      current_bind_group->textures
          .entries[current_bind_group->textures.length++] = *current_entry;
    }
  }
}

/**
   Add uniform of type Texture into the shader
   The function takes a "ready" texture view with a
   valid and already uploaded texture.

   In case one want to bind a raw picture/data (let's say imported from a file),
   one shall use the shader_add_texture() function that automatically
   handles the texture and texture view creation from the data.
 */
void shader_add_texture_view(shader *shader,
                             const ShaderCreateTextureViewDescriptor *desc) {

  if (shader_validate_binding(shader)) {
    ShaderBindGroup *current_bind_group =
        shader_get_bind_group(shader, desc->group_index);

    current_bind_group->visibility =
        desc->visibility | WGPUShaderStage_Fragment;

    for (int i = 0; i < desc->entry_count; i++) {

      if (current_bind_group->textures.length ==
          current_bind_group->textures.capacity) {
        VERBOSE_PRINT("Texture array reached maximum capacity\n");
        break;
      }

      // map the entry to bind group
      ShaderBindGroupTextureViewEntry *current_entry = &desc->entries[i];
      current_bind_group->textures
          .entries[current_bind_group->textures.length++] =
          (ShaderBindGroupTextureEntry){
              .texture_view = current_entry->texture_view,
              .binding = current_entry->binding,
              .dimension = current_entry->dimension,
              .format = current_entry->format,
              .sample_type = current_entry->sample_type,
          };
    }
  }
}

/**
   Add uniform of type Sampler into the shader
 */
void shader_add_sampler(shader *shader,
                        const ShaderCreateSamplerDescriptor *desc) {

  if (shader_validate_binding(shader)) {
    ShaderBindGroup *current_bind_group =
        shader_get_bind_group(shader, desc->group_index);

    current_bind_group->visibility =
        desc->visibility | WGPUShaderStage_Fragment;

    for (int i = 0; i < desc->entry_count; i++) {

      if (current_bind_group->samplers.length ==
          current_bind_group->samplers.capacity) {
        VERBOSE_PRINT("Sampler array reached maximum capacity\n");
        break;
      }

      // generate texture + sampler + texture view from data & size
      ShaderBindGroupSamplerEntry *current_entry = &desc->entries[i];

      // creating sampler by mapping desc configuration
      current_entry->sampler = wgpuDeviceCreateSampler(
          *shader->device, &(WGPUSamplerDescriptor){
                               .compare = current_entry->compare,
                               .addressModeU = current_entry->addressModeU,
                               .addressModeV = current_entry->addressModeV,
                               .addressModeW = current_entry->addressModeW,
                               .minFilter = current_entry->minFilter,
                               .magFilter = current_entry->magFilter,
                           });

      current_bind_group->samplers
          .entries[current_bind_group->samplers.length++] = *current_entry;
    }
  }
}

void shader_module_release(shader *shader) {
  // releasing shader module before drawing
  // invoked when adding the shader to the mesh (mesh_create)
  wgpuShaderModuleRelease(shader->module);
}

/**
   Check if a bind group in the shader isn't already registered
   if not, it creates a new bind group entry to the list
 */
ShaderBindGroup *shader_get_bind_group(shader *shader, size_t group_index) {

  int in = 0, i = 0;
  size_t index = shader->bind_groups.length;
  for (i = 0; i < shader->bind_groups.length; i++) {
    // check if group index is already in shader bind group index
    if (group_index == shader->bind_groups.entries[i].index) {
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
    shader->bind_groups.entries[shader->bind_groups.length].index = group_index;

    // create new bind group
    shader_bind_group_init(shader);

    shader->bind_groups.length++;
  }

  return &shader->bind_groups.entries[index];
}

/**
   Check if the shader has every requirements before binding groups
   Also checks if the bind groups array isn't already at full capacity
 */

// TODO add more validation by uniforms type (UNIFORM/ TEX/ SAMPLER...) check
// if it doesn't overflow with max accepted length
bool shader_validate_binding(shader *shader) {

  if (shader->device == NULL || shader->queue == NULL) {
    perror("Shader has no device or queue");
    return 0;
  }

  if (shader->bind_groups.length >= SHADER_MAX_BIND_GROUP) {
    perror("Bind group list at full capacity");
    return 0;
  }

  return 1;
}

/**
   Initialise shader bind group lists and eventually free/reset the existing
   ones if already existing
 */
void shader_bind_group_init(shader *shader) {

  // printf("init bind groups for %s\n", shader->name);
  ShaderBindGroupUniforms *uniform_group =
      &shader->bind_groups.entries[shader->bind_groups.length].uniforms;

  ShaderBindGroupTextures *texture_group =
      &shader->bind_groups.entries[shader->bind_groups.length].textures;

  ShaderBindGroupSamplers *sampler_group =
      &shader->bind_groups.entries[shader->bind_groups.length].samplers;

  /*NOTE:
    Max stack allocation easily reached with static allocation for
    Uniforms, Texture and Sampler arrays, so need to allocate them on the heap
  */

  // init Uniforms dynamic array
  uniform_group->length = 0;
  uniform_group->capacity = SHADER_UNIFORMS_DEFAULT_CAPACITY;
  uniform_group->entries = (ShaderBindGroupUniformEntry *)aligned_alloc(
      16,
      SHADER_UNIFORMS_DEFAULT_CAPACITY * sizeof(ShaderBindGroupUniformEntry));

  // init Texture dynamic array
  texture_group->length = 0;
  texture_group->capacity = SHADER_UNIFORMS_DEFAULT_CAPACITY;
  texture_group->entries = (ShaderBindGroupTextureEntry *)aligned_alloc(
      16,
      SHADER_UNIFORMS_DEFAULT_CAPACITY * sizeof(ShaderBindGroupTextureEntry));

  // init Sampler dynamic array
  sampler_group->length = 0;
  sampler_group->capacity = SHADER_UNIFORMS_DEFAULT_CAPACITY;
  sampler_group->entries = (ShaderBindGroupSamplerEntry *)aligned_alloc(
      16,
      SHADER_UNIFORMS_DEFAULT_CAPACITY * sizeof(ShaderBindGroupSamplerEntry));
}

/**
   Freeing all shader's bind groups allocation and reseting the length
 */
void shader_bind_group_clear(shader *shader) {

  for (size_t b = 0; b < shader->bind_groups.length; b++) {
    ShaderBindGroup *current_group = &shader->bind_groups.entries[b];
    current_group->bind_group = NULL;
    current_group->index = 0;

    // reseting uniforms
    current_group->uniforms.length = 0;

    // reseting textures
    if (current_group->textures.entries)
      free(current_group->textures.entries);

    current_group->textures.entries = NULL;
    current_group->textures.length = 0;
    current_group->textures.capacity = 0;

    // reseting samplers
    if (current_group->samplers.entries)
      free(current_group->samplers.entries);

    current_group->samplers.entries = NULL;
    current_group->samplers.length = 0;
    current_group->samplers.capacity = 0;
  }

  shader->bind_groups.length = 0;
}

pipeline *shader_pipeline(shader *shader) { return &shader->pipeline; }
