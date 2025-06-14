#ifndef _MESH_TOPOLOGY_WIREFRAME_H_
#define _MESH_TOPOLOGY_WIREFRAME_H_

#include "../../geometry/vertex/vertex.h"
#include "./base.h"
#include "core.h"

#define MESH_TOPOLOGY_WIREFRAME_ANCHOR_LIST_DEFAULT_CAPACITY 1023
#define MESH_TOPOLOGY_WIREFRAME_ANCHOR_DEFAULT_CAPACITY 32
#define MESH_TOPOLOGY_WIREFRAME_SUCCESS 0
#define MESH_TOPOLOGY_WIREFRAME_ALLOC_FAIL 1
#define MESH_TOPOLOGY_WIREFRAME_ERROR 2
#define MESH_TOPOLOGY_WIREFRAME_ANCHOR_UNSET 3

typedef union {
  vattr_t f;
  uint32_t u;
} MeshTopologyWireframeAnchorKey;

typedef struct {
  vindex_t *entries;
  size_t length;
  size_t capacity;
} MeshTopologyWireframeAnchor;

typedef struct {
  MeshTopologyWireframeAnchor *entries;
  size_t length;
  size_t capacity;
} MeshTopologyWireframeAnchorList;

typedef struct {
  VertexIndex index;
  VertexAttribute attribute;

  struct {
    MeshTopologyWireframeAnchorList hashed;
    MeshTopologyWireframeAnchorList mapped;
  } anchors;

} MeshTopologyWireframe;

int mesh_topology_wireframe_create(MeshTopology *, MeshTopologyWireframe *,
                                   const WGPUDevice *, const WGPUQueue *);

MeshTopology mesh_topology_wireframe_vertex(MeshTopologyWireframe *);

int mesh_topology_wireframe_update(const MeshTopologyBase *,
                                   MeshTopologyWireframe *, const WGPUQueue *);

#endif
