#ifndef _SHADER_H_
#define _SHADER_H_

#include <stddef.h>
#include <stdint.h>

#define SHADER_GROUP_CAMERA 0
#define SHADER_GROUP_VIEWPORT 0
#define SHADER_BIND_CAMERA 0
#define SHADER_BIND_VIEWPORT 1
#define SHADER_MAX_BIND_GROUP 12

#include "camera.h"
#include "viewport.h"
#include "webgpu/webgpu.h"

// descriptors
typedef struct {
  char *path;
  const char *label;
  WGPUDevice *device;
  WGPUQueue *queue;
  const char *name;
} ShaderCreateDescriptor;

// bind group

/* Takes in some data useful for the uniform update as well as the entry data
 * that will be overriden and uploaded to the GPU*/
typedef void (*UpdateCallback)(void *callback_data, void *entry_data);

typedef struct {
  uint32_t binding;
  WGPUBuffer buffer;
  uint64_t size;
  uint64_t offset;
  void *data;
  UpdateCallback update_callback;
  void *update_data;
} ShaderBindGroupEntry;

typedef struct {
  ShaderBindGroupEntry items[SHADER_MAX_BIND_GROUP];
  size_t length;
} ShaderBindGroupEntries;

typedef struct {
  WGPUBindGroup bind_group;        // bind group
  uint8_t index;                   // bind group id
  ShaderBindGroupEntries entries;  // entries (uniform)
  WGPUShaderStageFlags visibility; // visibility (frag | vert)
} ShaderBindGroup;

// uniform
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

// core
typedef struct {
  char *source; // shader source code
  WGPUShaderModule module;
  const char *name;

  // wgpu
  WGPUDevice *device;
  WGPUQueue *queue;
  WGPURenderPipeline pipeline;

  // vertex data
  struct {
    WGPUVertexAttribute attribute[2];
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
shader shader_create(const ShaderCreateDescriptor *);

void shader_add_uniform(shader *, const ShaderCreateUniformDescriptor *);

// on update
void shader_draw(shader *, WGPURenderPassEncoder *, const camera *,
                 const viewport *);

// release pipeline once setup is done (called in scene add mesh)
void shader_release(shader *);

// build up the whole pipeline
void shader_build(shader *);

#endif
