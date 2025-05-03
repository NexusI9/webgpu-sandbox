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

typedef enum {
  MESH_SHADER_DEFAULT,
  MESH_SHADER_SHADOW,
  MESH_SHADER_SOLID,
  MESH_SHADER_WIREFRAME,
  MESH_SHADER_CUSTOM
} MeshDrawMethod;

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
  struct mesh *entries;
  size_t *index;
  size_t capacity;
  size_t length;
} MeshIndexedList;

typedef struct {
  struct mesh *entries;
  size_t capacity;
  size_t length;
} MeshList;

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
  VertexAttribute vertex;

  // index list
  VertexIndex index;

  struct {
    WGPUBuffer vertex, index;
  } buffer;

  // shader
  struct {
    shader texture;
    shader shadow;
    shader wireframe;
  } shader;

  // hierarchy
  struct mesh *parent;
  MeshIndexedList children;

} mesh;

void mesh_create(mesh *, const MeshCreateDescriptor *);
void mesh_create_primitive(mesh *, const MeshCreatePrimitiveDescriptor *);

void mesh_set_vertex_attribute(mesh *, const VertexAttribute *);
void mesh_set_vertex_index(mesh *, const VertexIndex *);
void mesh_set_parent(mesh *, mesh *);
void mesh_set_name(mesh *, const char *);
void mesh_set_shader(mesh *, const ShaderCreateDescriptor *);

void mesh_create_vertex_buffer(mesh *, const MeshCreateBufferDescriptor *);
void mesh_create_index_buffer(mesh *, const MeshCreateBufferDescriptor *);

void mesh_draw(mesh *, MeshDrawMethod, WGPURenderPassEncoder *, const camera *,
               const viewport *);
void mesh_build(mesh *, MeshDrawMethod);

void mesh_scale(mesh *, vec3);
void mesh_position(mesh *, vec3);
void mesh_rotate(mesh *, vec3);
void mesh_rotate_quat(mesh *, versor);

mesh *mesh_new_child(mesh *);
mesh *mesh_new_child_empty(mesh *);
mesh *mesh_get_child_by_id(mesh *, size_t);

MeshUniform mesh_model_uniform(mesh *);

shader *mesh_shader_texture(mesh *);
shader *mesh_shader_shadow(mesh *);
shader *mesh_select_shader(mesh *, MeshDrawMethod);

#endif
