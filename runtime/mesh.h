#ifndef _MESH_H_
#define _MESH_H_

#include "../resources/primitive/primitive.h"
#include "camera.h"
#include "shader.h"
#include "vertex.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>

#define MESH_CHILD_LENGTH 12
#define MESH_NAME_MAX_LENGTH 64

// Builder Pattern | Descriptor Pattern
typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  vertex_attribute vertex;
  vertex_index index;
  shader shader;
  const char *name;
} MeshCreateDescriptor;

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  primitive primitive;
  shader shader;
  const char *name;
} MeshCreatePrimitiveDescriptor;

typedef struct {
  void *data;
  size_t size;
} MeshCreateBufferDescriptor;

typedef struct {
  mat4 model;
  vec4 position;
} MeshUniform;

// TODO: make it a linked list
typedef struct {
  struct mesh *items;
  size_t *index;
  size_t capacity;
  size_t length;
} mesh_list;

// Core
typedef struct mesh {

  size_t id;
  char *name;

  // transforms
  mat4 model;
  vec3 position;
  vec3 scale;
  vec3 rotation;

  // wgpu
  WGPUDevice *device;
  WGPUQueue *queue;

  // vertex list
  vertex_attribute vertex;

  // index list
  vertex_index index;

  struct {
    WGPUBuffer vertex, index;
  } buffer;

  // shader
  shader shader;

  // hierarchy
  struct mesh *parent;
  mesh_list children;

} mesh;

mesh mesh_create(const MeshCreateDescriptor *);
mesh mesh_create_primitive(const MeshCreatePrimitiveDescriptor *);

void mesh_set_vertex_attribute(mesh *, const vertex_attribute *);
void mesh_set_vertex_index(mesh *, const vertex_index *);
void mesh_set_parent(mesh *, mesh *);
void mesh_set_name(mesh *, const char *);
void mesh_set_shader(mesh *, const ShaderCreateDescriptor *);

void mesh_create_vertex_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_create_index_buffer(mesh *, const MeshCreateBufferDescriptor *);

void mesh_build(mesh *);
void mesh_draw(mesh *, WGPURenderPassEncoder *, const camera *,
               const viewport *);

void mesh_scale(mesh *, vec3);
void mesh_position(mesh *, vec3);
void mesh_rotate(mesh *, vec3);

size_t mesh_add_child(mesh *, mesh *);
size_t mesh_add_child_empty(mesh *);
mesh *mesh_get_child(mesh *, size_t);

MeshUniform mesh_model_uniform(mesh *);

// bind model, camera and viewport to bind group
void mesh_bind_matrices(mesh *, camera *, viewport *, uint8_t);

#endif
