#ifndef _MESH_TOPOLOGY_WIREFRAME_H_
#define _MESH_TOPOLOGY_WIREFRAME_H_

#include "../../geometry/vertex/vertex.h"
#include "core.h"

#define MESH_TOPOLOGY_WIREFRAME_ANCHOR_LIST_DEFAULT_CAPACITY 128
#define MESH_TOPOLOGY_WIREFRAME_ANCHOR_DEFAULT_CAPACITY 32
#define MESH_TOPOLOGY_WIREFRAME_SUCCESS 0
#define MESH_TOPOLOGY_WIREFRAME_ALLOC_FAIL 1
#define MESH_TOPOLOGY_WIREFRAME_ERROR 2

typedef struct {
  vindex_t anchor;
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
  MeshTopologyWireframeAnchorList anchors;
} MeshTopologyWireframe;

int mesh_topology_wireframe_create(MeshTopology *, MeshTopology *,
                                   const WGPUDevice *, const WGPUQueue *);

MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor(MeshTopologyWireframe *, const vindex_t);

VertexAttribute *
mesh_topology_wireframe_anchor_attribute(MeshTopologyWireframe *,
                                         const vindex_t);

void mesh_topology_wireframe_anchor_set_attribute(MeshTopologyWireframe *,
                                                  const vindex_t,
                                                  const VertexAttribute *);

int mesh_topology_wireframe_anchor_insert(MeshTopologyWireframeAnchor *,
                                          vindex_t);
int mesh_topology_wireframe_anchor_list_insert(
    MeshTopologyWireframeAnchorList *, MeshTopologyWireframeAnchor *);

MeshTopologyWireframeAnchor *
mesh_topology_wireframe_anchor_list_new(MeshTopologyWireframeAnchorList *);

MeshTopology mesh_topology_wireframe_vertex(MeshTopologyWireframe *);

#endif
