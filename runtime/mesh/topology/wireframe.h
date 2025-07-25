#ifndef _MESH_TOPOLOGY_WIREFRAME_H_
#define _MESH_TOPOLOGY_WIREFRAME_H_

#include "../../geometry/vertex/vertex.h"
#include "anchor.h"
#include "base.h"
#include "core.h"


typedef enum{
  MeshTopologyWireframeStatus_Success,
  MeshTopologyWireframeStatus_AllocFail,
  MeshTopologyWireframeStatus_UnderError,
} MeshTopologyWireframeStatus;


typedef struct {
  VertexIndex index;
  VertexAttribute attribute;
  MeshTopologyAnchorList anchors;
} MeshTopologyWireframe;

int mesh_topology_wireframe_create(MeshTopology *, MeshTopologyWireframe *,
                                   const WGPUDevice *, const WGPUQueue *);

MeshTopology mesh_topology_wireframe_vertex(MeshTopologyWireframe *);

int mesh_topology_wireframe_update(const MeshTopologyBase *,
                                   MeshTopologyWireframe *, const WGPUQueue *);

#endif
