#ifndef _SHADER_CORE_H_
#define _SHADER_CORE_H_

#include "../camera/camera.h"
#include "../pipeline/pipeline.h"
#include "../viewport/viewport.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>

// commons
#define SHADER_MAX_BIND_GROUP 4
#define SHADER_MAX_UNIFORMS 12
#define SHADER_UNIFORMS_DEFAULT_CAPACITY 8
#define SHADER_UNIFORM_STRUCT __attribute__((aligned(16)))

// texture shader
#define SHADER_TEXTURE_BINDGROUP_TEXTURES 0
#define SHADER_TEXTURE_BINDGROUP_VIEWS 1
#define SHADER_TEXTURE_BINDGROUP_LIGHTS 2

// wireframe shader
#define SHADER_WIREFRAME_BINDGROUP_VIEWS 0

// solid shader
#define SHADER_SOLID_BINDGROUP_VIEWS 0

// fixed shader
#define SHADER_FIXED_BINDGROUP_VIEWS 0

// path
#define SHADER_PATH_DEFAULT "./runtime/assets/shader/shader.default.wgsl"
#define SHADER_PATH_PBR "./runtime/assets/shader/shader.pbr.wgsl"
#define SHADER_PATH_SHADOW "./runtime/assets/shader/shader.shadow.wgsl"
#define SHADER_PATH_SCREEN "./runtime/assets/shader/shader.screen.wgsl"
#define SHADER_PATH_BILLBOARD "./runtime/assets/shader/shader.billboard.wgsl"
#define SHADER_PATH_LINE "./runtime/assets/shader/shader.line.wgsl"


// descriptors
typedef struct {
  char *path;
  char *label;
  const WGPUDevice *device;
  const WGPUQueue *queue;
  char *name;
} ShaderCreateDescriptor;

// bind group

/* Takes in some data useful for the uniform update as well as the entry data
 * that will be overriden and uploaded to the GPU*/
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
  ShaderUniformUpdate update;
  // private
  WGPUBuffer buffer;
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

// Bind group main
typedef struct {
  WGPUBindGroup bind_group;         // bind group
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
  const WGPUDevice *device;
  const WGPUQueue *queue;

  // pipelines
  Pipeline pipeline;

  // vertex data
  struct {
    WGPUVertexAttribute attribute[4];
    WGPUVertexBufferLayout layout;
  } vertex;

  // uniforms data along with userful information (buffer, group index...)
  // TODO: separate statics from dynamics
  // registered bind group unique indexes
  struct {
    ShaderBindGroup entries[SHADER_MAX_BIND_GROUP];
    size_t length;
  } bind_groups;

  int8_t projection_view_bindgroup;

} Shader;

// methods
void shader_create(Shader *, const ShaderCreateDescriptor *);
void shader_destroy(Shader *);

// on update
void shader_draw(Shader *, WGPURenderPassEncoder *, const Camera *,
                 const Viewport *);
void shader_uniform_update(ShaderBindGroup *, const WGPUQueue *);

void shader_module_release(Shader *);
Pipeline *shader_pipeline(Shader *);
void shader_pipeline_release_layout(Shader *);

#endif
