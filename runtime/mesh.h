#ifndef _MESH_H_
#define _MESH_H_

#include "../resources/geometry/vertex.h"
#include "../resources/primitive/primitive.h"
#include "camera.h"
#include "light.h"
#include "shader.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>

#define MESH_CHILD_LENGTH 6
#define MESH_NAME_MAX_LENGTH 64

// Builder Pattern | Descriptor Pattern
typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  VertexAttribute vertex;
  VertexIndex index;
  const char *name;
} MeshCreateDescriptor;

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  primitive primitive;
  const char *name;
} MeshCreatePrimitiveDescriptor;

typedef struct {
  void *data;
  size_t size;
} MeshCreateBufferDescriptor;

typedef struct {
  mat4 model;
  vec4 position;
} __attribute__((aligned(16))) MeshUniform;

// TODO: make it a linked list
typedef struct {
  struct mesh **entries;
  size_t *index;
  size_t capacity;
  size_t length;
} MeshIndexedList;

typedef struct {
  struct mesh *entries;
  size_t capacity;
  size_t length;
} MeshList;

typedef struct {
    VertexAttribute attribute;
    VertexIndex index;
} MeshVertex;

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

  // vertex data & buffer
  struct {
    MeshVertex base;
    MeshVertex wireframe;
  } vertex;

  // shader
  struct {
    shader texture;
    shader shadow;
    shader solid;
    shader wireframe;
  } shader;

  // hierarchy
  struct mesh *parent;
  MeshIndexedList children;

} mesh;

typedef shader *(*mesh_get_shader_callback)(mesh *);

void mesh_create(mesh *, const MeshCreateDescriptor *);
void mesh_create_primitive(mesh *, const MeshCreatePrimitiveDescriptor *);

void mesh_set_vertex_attribute(mesh *, const VertexAttribute *);
void mesh_set_vertex_index(mesh *, const VertexIndex *);
void mesh_set_parent(mesh *, mesh *);
void mesh_set_name(mesh *, const char *);
void mesh_set_shader(mesh *, const ShaderCreateDescriptor *);

void mesh_create_vertex_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_create_index_buffer(mesh *, const MeshCreateBufferDescriptor *);

void mesh_draw_default_buffer(mesh *, shader *, WGPURenderPassEncoder *,
                              const camera *, const viewport *);

void mesh_draw_wireframe_buffer(mesh *, shader *, WGPURenderPassEncoder *,
                                const camera *, const viewport *);

void mesh_build(mesh *, shader *);

void mesh_scale(mesh *, vec3);
void mesh_position(mesh *, vec3);
void mesh_rotate(mesh *, vec3);
void mesh_rotate_quat(mesh *, versor);

mesh *mesh_add_child(mesh *, mesh *);
mesh *mesh_new_child(mesh *);
mesh *mesh_new_child_empty(mesh *);
mesh *mesh_get_child_by_id(mesh *, size_t);

MeshUniform mesh_model_uniform(mesh *);

shader *mesh_shader_texture(mesh *);
shader *mesh_shader_shadow(mesh *);
shader *mesh_shader_wireframe(mesh *);
shader *mesh_shader_solid(mesh *);

#endif
