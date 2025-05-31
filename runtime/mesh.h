#ifndef _MESH_H_
#define _MESH_H_

#include "../resources/geometry/vertex.h"
#include "../resources/primitive/primitive.h"
#include "camera.h"
#include "shader.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>

#define MESH_CHILD_LENGTH 6
#define MESH_NAME_MAX_LENGTH 64
#define MESH_INDEX_FORMAT WGPUIndexFormat_Uint32

#define MESH_SUCCESS 0
#define MESH_ALLOC_FAILURE 1

// Builder Pattern | Descriptor Pattern
typedef struct {
  const WGPUDevice *device;
  const WGPUQueue *queue;
  VertexAttribute vertex;
  VertexIndex index;
  const char *name;
} MeshCreateDescriptor;

typedef struct {
  const WGPUDevice *device;
  const WGPUQueue *queue;
  Primitive primitive;
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
  struct Mesh **entries;
  size_t capacity;
  size_t length;
} MeshRefList;

typedef struct {
  struct Mesh *entries;
  size_t capacity;
  size_t length;
} MeshList;

typedef struct {
  VertexAttribute attribute;
  VertexIndex index;
} MeshVertex;

// Core
typedef struct Mesh {

  size_t id;
  char *name;

  // transforms
  mat4 model;
  vec3 position;
  vec3 scale;
  vec3 rotation;

  // wgpu
  const WGPUDevice *device;
  const WGPUQueue *queue;

  // vertex data & buffer
  struct {
    MeshVertex base;
    MeshVertex wireframe;
  } vertex;

  // shader
  struct {
    Shader texture;
    Shader shadow;
    Shader solid;
    Shader wireframe;
  } shader;

  // hierarchy
  struct Mesh *parent;
  MeshRefList children;

} Mesh;

typedef Shader *(*mesh_get_shader_callback)(Mesh *);
typedef MeshVertex *(*mesh_get_vertex_callback)(Mesh *);

void mesh_create(Mesh *, const MeshCreateDescriptor *);
void mesh_create_primitive(Mesh *, const MeshCreatePrimitiveDescriptor *);

void mesh_set_vertex_attribute(Mesh *, const VertexAttribute *);
void mesh_set_vertex_index(Mesh *, const VertexIndex *);
void mesh_set_parent(Mesh *, Mesh *);
void mesh_set_name(Mesh *, const char *);
void mesh_set_shader(Mesh *, const ShaderCreateDescriptor *);

void mesh_create_vertex_buffer(Mesh *, const MeshCreateBufferDescriptor *);
void mesh_create_index_buffer(Mesh *, const MeshCreateBufferDescriptor *);

void mesh_draw(MeshVertex *, Shader *, WGPURenderPassEncoder *, const Camera *,
               const Viewport *);

void mesh_build(Mesh *, Shader *);

void mesh_scale(Mesh *, vec3);
void mesh_position(Mesh *, vec3);
void mesh_rotate(Mesh *, vec3);
void mesh_rotate_quat(Mesh *, versor);

Mesh *mesh_add_child(Mesh *, Mesh *);
Mesh *mesh_new_child(Mesh *);
Mesh *mesh_new_child_empty(Mesh *);
Mesh *mesh_get_child_by_id(Mesh *, size_t);

MeshUniform mesh_model_uniform(Mesh *);

MeshVertex *mesh_vertex_base(Mesh *);
MeshVertex *mesh_vertex_wireframe(Mesh *);

Shader *mesh_shader_texture(Mesh *);
Shader *mesh_shader_shadow(Mesh *);
Shader *mesh_shader_wireframe(Mesh *);
Shader *mesh_shader_solid(Mesh *);

int mesh_list_create(MeshList *, size_t);
Mesh *mesh_list_insert(MeshList *);

int mesh_reference_list_create(MeshRefList *, size_t);
Mesh *mesh_reference_list_insert(MeshRefList *, Mesh *);
int mesh_reference_list_transfert(MeshRefList *, MeshRefList *);
int mesh_reference_list_copy(const MeshRefList *, MeshRefList *);
#endif
