#ifndef _MESH_TOPOLOGY_CORE_H_
#define _MESH_TOPOLOGY_CORE_H_

#include "../../geometry/vertex/vertex.h"

typedef struct{
    VertexIndex* index;
    VertexAttribute* attribute;
} MeshTopology;

#endif
