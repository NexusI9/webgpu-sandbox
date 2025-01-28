#ifndef _MESH_H_
#define _MESH_H_

#include "../backend/state.h"
#include "shader.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

typedef struct {

  WGPUDevice *device;
  WGPUQueue *queue;

} MeshWGPU;

// Builder Pattern | Descriptor Pattern
typedef struct {

  // wgpu
  MeshWGPU wgpu;

  struct {
    const float *data;
    uint16_t length;
  } vertex;

  struct {
    const uint16_t *data;
    uint16_t length;
  } index;

  shader shader;

} MeshCreateDescriptor;

typedef struct {
  void *data;
  size_t size;
} MeshCreateBufferDescriptor;

// Core
typedef struct {

  uint8_t id;

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
    WGPUBuffer vertex, index, uniform;
  } buffer;

  // shader
  shader shader;

} mesh;

mesh mesh_create(const MeshCreateDescriptor *);
void mesh_create_vertex_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_create_index_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_create_uniform_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_draw(const mesh *, WGPURenderPassEncoder *, const camera *, const viewport *);

#endif
