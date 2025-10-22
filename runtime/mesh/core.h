#ifndef _MESH_CORE_H_
#define _MESH_CORE_H_

#include <cglm/types.h>
#include <stddef.h>
#include <stdint.h>

#include "backend/registry.h"
#include "backend/ssbo.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/geometry/vertex/vertex.h"
#include "runtime/primitive/core.h"
#include "runtime/primitive/primitive.h"
#include "runtime/shader/core.h"
#include "runtime/shader/shader.h"
#include "topology/base.h"
#include "topology/boundbox.h"
#include "topology/core.h"
#include "topology/topology.h"
#include "topology/wireframe.h"
#include "utils/dyli.h"
#include "utils/name.h"
#include "webgpu/webgpu.h"
#include "utils/defines.h"

#define MESH_CHILD_LENGTH 6
#define MESH_NAME_MAX_LENGTH 64
#define MESH_INDEX_FORMAT WGPUIndexFormat_Uint32

typedef struct Mesh Mesh;

typedef enum {
  MeshStatus_Success,
  MeshStatus_AllocFail,
  MeshStatus_InvalidShaderIndex,
  MeshStatus_AlreadyCreated,
  MeshStatus_UndefError,
} MeshStatus;

typedef struct {
  struct Mesh **entries;
  size_t capacity;
  size_t length;
} MeshRefList;

#define MESH_STD_SHADER_COUNT 8

typedef enum {
  MeshShader_Texture,
  MeshShader_Fixed,
  MeshShader_Shadow,
  MeshShader_Reflection,
  MeshShader_Solid,
  MeshShader_Wireframe,
  MeshShader_Outline,
  MeshShader_Stencil,
} MeshShader;

// Builder Pattern | Descriptor Pattern
typedef struct {
  VertexAttribute vertex;
  VertexIndex index;
  const char *name;
} MeshCreateDescriptor;

typedef struct {
  Primitive *primitive;
  const char *name;
} MeshCreatePrimitiveDescriptor;

typedef struct {
  mat4 model;
  vec4 position;
  uint32_t probe_reflection_plane_count;
  uint32_t probe_reflection_grid_count;
  uint32_t _pad[42];
} __attribute__((aligned(16))) MeshUniform;

typedef void (*mesh_get_transform_attribute)(Mesh *, vec3);

// Core
struct Mesh {

  reg_id_t id;
  name_t name;

  // transforms
  mat4 model;
  vec3 position;
  vec3 scale;
  vec3 rotation_euler;
  versor rotation_quat;

  SSBOSlot ssbo_slot;

  // vertex data & buffer topology
  struct mesh_topology {
    MeshTopologyBase base;
    MeshTopologyWireframe wireframe;
    MeshTopologyBoundbox boundbox;
    MeshTopology override;
  } topology;

  // shader
  struct mesh_shader {
    Shader standard[MESH_STD_SHADER_COUNT];
    Shader *active;
  } shader;

  // hierarchy
  Mesh *parent;
  MeshRefList children;
};

EXTERN_C_BEGIN

// constructor
void mesh_create(Mesh *, const MeshCreateDescriptor *);
void mesh_create_primitive(Mesh *, const MeshCreatePrimitiveDescriptor *);

// shader
void mesh_draw(MeshTopology, Shader *, WGPURenderPassEncoder);

// hierarchy
void mesh_set_parent(Mesh *, Mesh *);
DynamicListStatus mesh_child_add(Mesh *, Mesh *);
DynamicListStatus mesh_child_remove(Mesh *, Mesh *);
Mesh *mesh_child_new(Mesh *);

// topology
typedef MeshTopology (*mesh_get_topology_callback)(Mesh *);
typedef int (*mesh_topology_create_callback)(MeshTopology *, MeshTopology *);

// accessor

/**
   Retireve the mesh children address at the given index from the mesh children
   list
 */
static inline Mesh *mesh_child_get_by_id(Mesh *mesh, size_t index) {
  return mesh->children.entries[index];
}

/**
   Return Mesh Base Vertex
 */
static inline MeshTopology mesh_topology_base(Mesh *mesh) {
  return mesh_topology_base_vertex(&mesh->topology.base);
}

/**
   Return Mesh Wireframe Vertex
 */
static inline MeshTopology mesh_topology_wireframe(Mesh *mesh) {
  return mesh_topology_wireframe_vertex(&mesh->topology.wireframe);
}

/**
   Return Mesh Boundbox Vertex
 */
static inline MeshTopology mesh_topology_boundbox(Mesh *mesh) {
  return mesh_topology_boundbox_vertex(&mesh->topology.boundbox);
}

/**
   Override topology is primarily used for fixed mesh during the scene build and
   draw phase and will be the targeted topology for whatever render mode
   (solid/wireframe/texture)
 */
static inline MeshTopology mesh_topology_override(Mesh *mesh) {
  return mesh->topology.override;
}

/**
   Define the override topology.
 */
static inline void mesh_topology_set_override(Mesh *mesh,
                                              const MeshTopology topology) {
  mesh->topology.override = topology;
}

static inline void mesh_get_position(Mesh *mesh, vec3 dest) {
  glm_vec3_copy(mesh->position, dest);
}

static inline void mesh_get_scale(Mesh *mesh, vec3 dest) {
  glm_vec3_copy(mesh->scale, dest);
}

static inline void mesh_get_rotation_euler(Mesh *mesh, vec3 dest) {
  glm_vec3_copy(mesh->rotation_euler, dest);
}

static const char *mesh_get_name(Mesh *mesh) { return mesh->name; }

static inline void mesh_set_name(Mesh *mesh, const char *src) {
  name_copy(src, mesh->name);
}

EXTERN_C_END

#endif
