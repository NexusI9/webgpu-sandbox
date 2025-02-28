#ifndef _MESH_H_
#define _MESH_H_

#include "../resources/primitive/primitive.h"
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

  vertex vertex;
  index index;

  shader shader;

} MeshCreateDescriptor;

typedef struct {

  // wgpu
  MeshWGPU wgpu;
    
  primitive primitive;
    
  shader shader;

} MeshPrimitiveCreateDescriptor;

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
    WGPUBuffer vertex, index;
  } buffer;

  // shader
  shader shader;

} mesh;

// create mesh from custom vertex and index array
mesh mesh_create(const MeshCreateDescriptor *);

// create mesh from primitive (preset)
mesh mesh_create_primitive(const MeshPrimitiveCreateDescriptor *);

void mesh_create_vertex_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_create_index_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_draw(mesh *, WGPURenderPassEncoder *, const camera *,
               const viewport *);

#endif
