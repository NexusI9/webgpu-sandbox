#ifndef _SHADER_H_
#define _SHADER_H_

#include <stddef.h>
#include <stdint.h>

#define SHADER_GROUP_CAMERA 0
#define SHADER_GROUP_VIEWPORT 0
#define SHADER_BIND_CAMERA 0
#define SHADER_BIND_VIEWPORT 1
#define SHADER_MAX_BIND_GROUP 12

#define SHADER_MAX_UNIFORMS 12
#define SHADER_UNIFORMS_DEFAULT_CAPACITY 8

#include "camera.h"
#include "viewport.h"
#include "webgpu/webgpu.h"

// descriptors
typedef struct {
  char *path;
  char *label;
  WGPUDevice *device;
  WGPUQueue *queue;
  char *name;
} ShaderCreateDescriptor;

// bind group

/* Takes in some data useful for the uniform update as well as the entry data
 * that will be overriden and uploaded to the GPU*/
typedef void (*UpdateCallback)(void *callback_data, void *entry_data);

typedef struct {
  uint32_t binding;
  uint64_t size;
  uint64_t offset;
  void *data;
  UpdateCallback update_callback;
  void *update_data;
  // private
  WGPUBuffer buffer;
} ShaderBindGroupEntry;

typedef struct {
  uint32_t binding;
  int width;
  int height;
  unsigned char *data;
  size_t size;
  // private
  WGPUTextureView texture_view;
} ShaderBindGroupTextureEntry;

typedef struct {
  uint32_t binding;
  WGPUAddressMode addressModeU;
  WGPUAddressMode addressModeV;
  WGPUAddressMode addressModeW;
  WGPUFilterMode minFilter;
  WGPUFilterMode magFilter;
  // private
  WGPUSampler sampler;
} ShaderBindGroupSamplerEntry;

// uniform / texture / sampler array
typedef struct {
  ShaderBindGroupEntry items[SHADER_MAX_UNIFORMS];
  size_t length;
} ShaderBindGroupUniforms;

typedef struct {
  ShaderBindGroupTextureEntry *items;
  size_t capacity;
  size_t length;
} ShaderBindGroupTextures;

typedef struct {
  ShaderBindGroupSamplerEntry *items;
  size_t capacity;
  size_t length;
} ShaderBindGroupSamplers;

// Bind group main
typedef struct {
  WGPUBindGroup bind_group;         // bind group
  uint8_t index;                    // bind group id
  WGPUShaderStageFlags visibility;  // visibility (frag | vert)
                                    // ELEMENTS:
  ShaderBindGroupUniforms uniforms; // uniforms
  ShaderBindGroupTextures textures; // textures
  ShaderBindGroupSamplers samplers; // samplers
} ShaderBindGroup;

// Descriptors
typedef struct {
  CameraUniform uCamera;
  ViewportUniform uViewport;
} ShaderViewProjectionUniform;

typedef struct {
  uint8_t group_index;
  uint8_t entry_count;
  ShaderBindGroupEntry *entries;
  WGPUShaderStageFlags visibility;
} ShaderCreateUniformDescriptor;

typedef struct {
  uint8_t group_index;
  uint8_t entry_count;
  ShaderBindGroupTextureEntry *entries;
  WGPUShaderStageFlags visibility;
} ShaderCreateTextureDescriptor;

typedef struct {
  uint8_t group_index;
  uint8_t entry_count;
  ShaderBindGroupSamplerEntry *entries;
  WGPUShaderStageFlags visibility;
} ShaderCreateSamplerDescriptor;

// pbr uniforms
typedef struct {
  vec4 diffuse_factor;
  float metallic_factor;
  float roughness_factor;
  float occlusion_factor;
  float normal_scale;
  vec3 emissive_factor;
  float _padding;
} ShaderPBRUniform;

// core
typedef struct {
  char *source; // shader source code
  WGPUShaderModule module;
  char *name;

  // wgpu
  WGPUDevice *device;
  WGPUQueue *queue;

  // pipeline
  struct {
    WGPURenderPipeline handle;
    WGPUPipelineLayout layout;
  } pipeline;

  // vertex data
  struct {
    WGPUVertexAttribute attribute[4];
    WGPUVertexBufferLayout layout;
  } vertex;

  // uniforms data along with userful information (buffer, group index...)
  // TODO: separate statics from dynamics
  // registered bind group unique indexes
  struct {
    ShaderBindGroup items[SHADER_MAX_BIND_GROUP];
    size_t length;
  } bind_groups;

  int8_t projection_view_bindgroup;

} shader;

// methods
void shader_create(shader *, const ShaderCreateDescriptor *);

void shader_add_uniform(shader *, const ShaderCreateUniformDescriptor *);
void shader_add_texture(shader *, const ShaderCreateTextureDescriptor *);
void shader_add_sampler(shader *, const ShaderCreateSamplerDescriptor *);
// on update
void shader_draw(shader *, WGPURenderPassEncoder *, const camera *,
                 const viewport *);

// build up the whole pipeline
void shader_build(shader *);

#endif
