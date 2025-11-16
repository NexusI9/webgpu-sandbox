#ifndef _MESH_TOPOLOGY_CORE_H_
#define _MESH_TOPOLOGY_CORE_H_

#include "runtime/geometry/vertex/vertex.h"

typedef struct {
  VertexIndex *index;
  VertexAttribute *attribute;
} MeshTopology;

typedef enum {
  MeshTopologyType_Base,
  MeshTopologyType_Wireframe,
  MeshTopologyType_BoundingBox,
} MeshTopologyType;

#endif
