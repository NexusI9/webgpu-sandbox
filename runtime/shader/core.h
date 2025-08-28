#ifndef _SHADER_CORE_H_
#define _SHADER_CORE_H_

#include "../runtime/pipeline/pipeline.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>

// commons
#define SHADER_MAX_BIND_GROUP 4
#define SHADER_MAX_UNIFORMS 12
#define SHADER_MAX_OFFSET_CAPACITY 8
#define SHADER_UNIFORMS_DEFAULT_CAPACITY 8
#define SHADER_UNIFORM_STRUCT __attribute__((aligned(16)))

// texture shader
// according to PBR.wgsl
#define SHADER_TEXTURE_BINDGROUP_VIEWS 0
#define SHADER_TEXTURE_BINDGROUP_TEXTURES 1
#define SHADER_TEXTURE_BINDING_AO 8

#define SHADER_TEXTURE_BINDGROUP_LIGHTS 2
#define SHADER_TEXTURE_BINDING_POINT_TEXTURE_MAP 5
#define SHADER_TEXTURE_BINDING_DIR_TEXTURE_MAP 7

// wireframe shader
#define SHADER_WIREFRAME_BINDGROUP_VIEWS 0

// solid shader
#define SHADER_SOLID_BINDGROUP_VIEWS 0

// fixed shader
#define SHADER_FIXED_BINDGROUP_VIEWS 0

// descriptors
typedef struct {
  const char *path;
  const char *label;
  WGPUDevice device;
  WGPUQueue queue;
  const char *name;
  const Pipeline *pipeline;
} ShaderCreateDescriptor;

// bind group
/**

   Takes in some data useful for the uniform update as well as the entry data
   that will be overriden and uploaded to the GPU.

   Trigger used for the mesh model uniform. It compare the current model with
   the newest provided and return true if it's different.
   When a trigger returns true the uniform gets replaced by the uniform
   callback (function above "mesh_uniform_model_update").

   Having a trigger prevent to constantly rewrite in the GPU if the values are
   the same.

   The "callback_data" corressponds to the data used in both Callback and
   Trigger. It can be anything, but it's the reference from which the
   "entry_data" will be compared to. The "entry_data" however represent the data
   pass as the unifom.

   A common usage is:
   - Callback Data: object itself (Camera*, Mesh*...)
   - Entry Data: object's uniform (model view matrix...)

   .--------------------------------------------------------------------------.
   |                                                                          |
   |   (ShaderBindGroupUniformEntry) {                                        |
   |       ...                                                                |
   |       .data = &uniformObjectStruct,                                      |
   |       .update =                                                          |
   |          {                                                               |
   |              .callback = update_function,                                |
   |              .trigger = trigger_function,                                |
   |              .data = object*                                             |
   |          }                                                               |
   |   }                                                                      |
   |                                                                          |
   '--------------------------------------------------------------------------'

   In this way we can compare the uniform state (stored in GPU) with the object
   latest state (updated CPU side) and update it accordinly.

 */
typedef void (*shader_uniform_update_callback)(void *callback_data,
                                               void *entry_data);

typedef bool (*shader_uniform_update_trigger)(void *callback_data,
                                              const void *entry_data);

typedef uint32_t shader_binding_t;
typedef uint8_t shader_bindgroup_t;

typedef struct {
  int8_t *textures;
  int8_t *views;
  int8_t *lights;
  int8_t *shadows;
} ShaderBindGroupIndexes;

typedef struct {
  shader_uniform_update_callback callback;
  shader_uniform_update_trigger trigger;
  void *data;
} ShaderUniformUpdate;

typedef struct {
  uint32_t binding;
  uint64_t size;
  uint64_t offset;
  void *data;
  WGPUBuffer buffer;
  WGPUBufferUsage usage;
  ShaderUniformUpdate update;
} ShaderBindGroupUniformEntry;

typedef struct {
  shader_binding_t binding;
  int width;
  int height;
  unsigned char *data;
  size_t size;
  uint8_t channels;
  WGPUTextureViewDimension dimension;
  WGPUTextureFormat format;
  WGPUTextureSampleType sample_type;
  // private
  WGPUTextureView texture_view;
} ShaderBindGroupTextureEntry;

typedef struct {
  shader_binding_t binding;
  WGPUTextureViewDimension dimension;
  WGPUTextureView texture_view;
  WGPUTextureFormat format;
  WGPUTextureSampleType sample_type;
} ShaderBindGroupTextureViewEntry;

typedef struct {
  shader_binding_t binding;
  WGPUAddressMode addressModeU;
  WGPUAddressMode addressModeV;
  WGPUAddressMode addressModeW;
  WGPUFilterMode minFilter;
  WGPUFilterMode magFilter;
  WGPUSamplerBindingType type;
  WGPUCompareFunction compare;
  // private
  WGPUSampler sampler;
} ShaderBindGroupSamplerEntry;

// uniform / texture / sampler array
typedef struct {
  ShaderBindGroupUniformEntry *entries;
  size_t length;
  size_t capacity;
} ShaderBindGroupUniforms;

typedef struct {
  ShaderBindGroupTextureEntry *entries;
  size_t capacity;
  size_t length;
} ShaderBindGroupTextures;

typedef struct {
  ShaderBindGroupSamplerEntry *entries;
  size_t capacity;
  size_t length;
} ShaderBindGroupSamplers;

// dynamic lists
// uniform / texture / samplers pointers array
typedef struct {
  ShaderBindGroupUniformEntry **entries;
  size_t length;
  size_t capacity;
} ShaderBindGroupUniformsDynamics;

typedef struct {
  ShaderBindGroupTextureEntry **entries;
  size_t capacity;
  size_t length;
} ShaderBindGroupTexturesDynamics;

typedef struct {
  ShaderBindGroupSamplerEntry **entries;
  size_t capacity;
  size_t length;
} ShaderBindGroupSamplersDynamics;

// Bind group main
typedef struct {
  WGPUBindGroup bind_group;        // bind group
  WGPUShaderStageFlags visibility; // visibility (frag | vert)

  struct {
    uint8_t count;
    uint32_t entries[SHADER_MAX_OFFSET_CAPACITY];
  } offset;

  ShaderBindGroupUniforms uniforms;                  // uniforms
  ShaderBindGroupUniformsDynamics uniforms_dynamics; // dynamic pointers

  ShaderBindGroupTextures textures;                  // textures
  ShaderBindGroupTexturesDynamics textures_dynamics; // dynamic pointers

  ShaderBindGroupSamplers samplers;                  // sampler
  ShaderBindGroupSamplersDynamics samplers_dynamics; // sampler

} ShaderBindGroup;

typedef struct {
  ShaderBindGroup entries[SHADER_MAX_BIND_GROUP];
  size_t length;
} ShaderBindGroupList;

// Descriptors

typedef struct {
  shader_bindgroup_t group_index;
  shader_bindgroup_t entry_count;
  ShaderBindGroupUniformEntry *entries;
  WGPUShaderStageFlags visibility;
} ShaderCreateUniformDescriptor;

typedef struct {
  shader_bindgroup_t group_index;
  shader_bindgroup_t entry_count;
  ShaderBindGroupTextureEntry *entries;
  WGPUShaderStageFlags visibility;
} ShaderCreateTextureDescriptor;

typedef struct {
  shader_bindgroup_t group_index;
  shader_bindgroup_t entry_count;
  ShaderBindGroupTextureViewEntry *entries;
  WGPUShaderStageFlags visibility;
} ShaderCreateTextureViewDescriptor;

typedef struct {
  shader_bindgroup_t group_index;
  shader_bindgroup_t entry_count;
  ShaderBindGroupSamplerEntry *entries;
  WGPUShaderStageFlags visibility;
} ShaderCreateSamplerDescriptor;

// core
typedef struct {
  char *name;

  // wgpu
  WGPUDevice device;
  WGPUQueue queue;

  // pipelines
  const Pipeline *pipeline;

  ShaderBindGroupList bind_groups;

} Shader;

// methods
void shader_create(Shader *, const ShaderCreateDescriptor *);
void shader_destroy(Shader *);
void shader_build(Shader *);

// on update
void shader_draw(Shader *, WGPURenderPassEncoder);
void shader_uniform_update(ShaderBindGroup *, const WGPUQueue);

void shader_module_release(Shader *);
const Pipeline *shader_pipeline(Shader *);
void shader_pipeline_release_layout(Shader *);

#endif
