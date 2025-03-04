#ifndef _MESH_H_
#define _MESH_H_

#include "../resources/primitive/primitive.h"
#include "camera.h"
#include "shader.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

typedef struct {

  WGPUDevice *device;
  WGPUQueue *queue;

} MeshWGPU;

// Builder Pattern | Descriptor Pattern
typedef struct {
  MeshWGPU wgpu;
  vertex vertex;
  index index;
  shader shader;
} MeshCreateDescriptor;

typedef struct {
  MeshWGPU wgpu;
  primitive primitive;
  shader shader;
} MeshCreatePrimitiveDescriptor;

typedef struct {
  void *data;
  size_t size;
} MeshCreateBufferDescriptor;

typedef struct {
  mat4 model;
  vec4 position;
} MeshUniform;

// Core
typedef struct {

  uint8_t id;

  // transforms
  mat4 model;
  vec3 position;
  vec3 scale;
  vec3 rotation;

  // wgpu
  MeshWGPU wgpu;

  // vertex list

  struct {
    const float *data;
    int16_t length;
  } vertex;

  // index list
  struct {
    const uint16_t *data;
    uint16_t length;
  } index;

  struct {
    WGPUBuffer vertex, index;
  } buffer;

  // shader
  shader shader;

} mesh;

mesh mesh_create(const MeshCreateDescriptor *);
mesh mesh_create_primitive(const MeshCreatePrimitiveDescriptor *);
void mesh_create_vertex_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_create_index_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_draw(mesh *, WGPURenderPassEncoder *, const camera *,
               const viewport *);

void mesh_scale(mesh *, vec3);
void mesh_position(mesh *, vec3);
void mesh_rotate(mesh *, vec3);

MeshUniform mesh_model_uniform(mesh *);

// bind model, camera and viewport to bind group
void mesh_bind_matrices(mesh *, camera *, viewport *, uint8_t);
#endif
