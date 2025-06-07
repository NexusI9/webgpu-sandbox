#ifndef _MESH_TOPOLOGY_BASE_H_
#define _MESH_TOPOLOGY_BASE_H_
#include "../../geometry/vertex/vertex.h"
#include "core.h"

typedef struct {
  VertexAttribute attribute;
  VertexIndex index;
} MeshTopologyBase;

MeshTopology mesh_topology_base_vertex(MeshTopologyBase *);

#endif
