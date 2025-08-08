#ifndef _MESH_CORE_H_
#define _MESH_CORE_H_

#include "../backend/registry.h"
#include "../geometry/vertex/vertex.h"
#include "../primitive/primitive.h"
#include "../shader/shader.h"
#include "./topology/topology.h"
#include "topology/boundbox.h"
#include "topology/core.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>

#define MESH_CHILD_LENGTH 6
#define MESH_NAME_MAX_LENGTH 64
#define MESH_INDEX_FORMAT WGPUIndexFormat_Uint32
#define MESH_STD_SHADER_COUNT 5

typedef struct Mesh Mesh;

typedef enum {
  MeshStatus_Success,
  MeshStatus_AllocFail,
} MeshStatus;

typedef struct {
  struct Mesh **entries;
  size_t capacity;
  size_t length;
} MeshRefList;

typedef enum {
  MeshShader_Texture,
  MeshShader_Shadow,
  MeshShader_Solid,
  MeshShader_Wireframe,
  MeshShader_Fixed,
} MeshShader;

// Builder Pattern | Descriptor Pattern
typedef struct {
  WGPUDevice device;
  WGPUQueue queue;
  VertexAttribute vertex;
  VertexIndex index;
  const char *name;
} MeshCreateDescriptor;

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  Primitive primitive;
  const char *name;
} MeshCreatePrimitiveDescriptor;

typedef void (*mesh_get_transform_attribute)(Mesh *, vec3 *);

// Core
struct Mesh {

  id_t id;
  char *name;

  // transforms
  mat4 model;
  vec3 position;
  vec3 scale;
  vec3 rotation_euler;
  versor rotation_quat;

  // wgpu
  WGPUDevice device;
  WGPUQueue queue;

  // vertex data & buffer topology
  struct {
    MeshTopologyBase base;
    MeshTopologyWireframe wireframe;
    MeshTopologyBoundbox boundbox;
    MeshTopology override;
  } topology;

  // shader
  struct {
    Shader standard[MESH_STD_SHADER_COUNT];
    Shader *active;
  } shader;

  // hierarchy
  Mesh *parent;
  MeshRefList children;
};

// constructor
void mesh_create(Mesh *, const MeshCreateDescriptor *);
void mesh_create_primitive(Mesh *, const MeshCreatePrimitiveDescriptor *);
void mesh_set_name(Mesh *, const char *);

// shader
void mesh_draw(MeshTopology, Shader *, WGPURenderPassEncoder *);

// hierarchy
void mesh_set_parent(Mesh *, Mesh *);
Mesh *mesh_add_child(Mesh *, Mesh *);
Mesh *mesh_new_child(Mesh *);
Mesh *mesh_new_child_empty(Mesh *);
Mesh *mesh_get_child_by_id(Mesh *, size_t);

// topology
typedef MeshTopology (*mesh_get_topology_callback)(Mesh *);
typedef int (*mesh_topology_create_callback)(MeshTopology *, MeshTopology *,
                                             const WGPUDevice, const WGPUQueue);

MeshTopology mesh_topology_base(Mesh *);
MeshTopology mesh_topology_wireframe(Mesh *);
MeshTopology mesh_topology_boundbox(Mesh *);
MeshTopology mesh_topology_override(Mesh *); // for fixed mesh only
void mesh_topology_set_override(Mesh *, const MeshTopology topology);

// getter
void mesh_get_position(Mesh *, vec3 *);
void mesh_get_scale(Mesh *, vec3 *);
void mesh_get_rotation_euler(Mesh *, vec3 *);

#endif
