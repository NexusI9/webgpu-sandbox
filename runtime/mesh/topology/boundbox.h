#ifndef _MESH_TOPOLOGY_BOUNDBOX_H_
#define _MESH_TOPOLOGY_BOUNDBOX_H_
#include "../../geometry/aabb/aabb.h"
#include "anchor.h"
#include "base.h"

#define MESH_TOPOLOGY_BOUNDBOX_SUCCESS 0
#define MESH_TOPOLOGY_BOUNDBOX_ALLOC_FAIL 1
#define MESH_TOPOLOGY_BOUNDBOX_ERROR 2

typedef struct {
  VertexIndex index;
  VertexAttribute attribute;
  vec3 corners[8];
  AABB bound;
} MeshTopologyBoundbox;

void mesh_topology_boundbox_compute_bound(MeshTopologyBase *, mat4,
                                          MeshTopologyBoundbox *);

int mesh_topology_boundbox_create(MeshTopologyBase *, mat4,
                                  MeshTopologyBoundbox *, const WGPUDevice *,
                                  const WGPUQueue *);

MeshTopology mesh_topology_boundbox_vertex(MeshTopologyBoundbox *);

int mesh_topology_boundbox_update(const MeshTopologyBase *,
                                  MeshTopologyBoundbox *, const WGPUQueue *);

#endif
