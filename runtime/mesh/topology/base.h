#ifndef _MESH_TOPOLOGY_BASE_H_
#define _MESH_TOPOLOGY_BASE_H_
#include "../../geometry/vertex/vertex.h"
#include "anchor.h"
#include "core.h"

#define MESH_TOPOLOGY_BASE_SUCCESS 0
#define MESH_TOPOLOGY_BASE_ALLOC_FAIL 1
#define MESH_TOPOLOGY_BASE_ERROR 2
#define MESH_TOPOLOGY_BASE_EMPTY 3

typedef struct {
  VertexAttribute attribute;
  VertexIndex index;

  MeshTopologyAnchorList siblings;

} MeshTopologyBase;

MeshTopology mesh_topology_base_vertex(MeshTopologyBase *);

void mesh_topology_base_create(MeshTopologyBase *, VertexAttribute *,
                               VertexIndex *, const WGPUDevice *,
                               const WGPUQueue *);

int mesh_topology_base_create_vertex_attribute(MeshTopologyBase *,
                                               const VertexAttribute *,
                                               const WGPUDevice *,
                                               const WGPUQueue *);

int mesh_topology_base_create_vertex_index(MeshTopologyBase *,
                                           const VertexIndex *,
                                           const WGPUDevice *,
                                           const WGPUQueue *);

void mesh_topology_base_scale(MeshTopologyBase *,
                              const VertexIndexSelection *, vec3 *);

#endif
