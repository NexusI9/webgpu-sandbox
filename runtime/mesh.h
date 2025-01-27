#ifndef _MESH_H_
#define _MESH_H_

#include <stdint.h>
#include "webgpu/webgpu.h"
#include "../backend/state.h"
#include "shader.h"

// Builder Pattern | Descriptor Pattern
typedef struct {

  struct {
    const float *data;
    uint16_t length;
  } vertex;

  struct {
    const uint16_t *data;
    uint16_t length;
  } index;

} MeshCreateDescriptor;

typedef struct {

  uint8_t id;

  // vertex list
  struct {
    const float *data;
    uint16_t length;
  } vertex;

  // index list
  struct {
    const uint16_t *data;
    uint16_t length;
  } index;

  struct {
      WGPUBuffer vertex, index, uniform;
  } buffer;

   

} mesh;

mesh mesh_create(const MeshCreateDescriptor *);
WGPUBuffer mesh_create_vertex_buffer(state_t, void*, size_t);


#endif
