#ifndef _MESH_TOPOLOGY_BOUNDBOX_H_
#define _MESH_TOPOLOGY_BOUNDBOX_H_
#include <cglm/types.h>
#include <webgpu/webgpu.h>

#include "runtime/geometry/aabb/aabb.h"
#include "anchor.h"
#include "base.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "core.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"

typedef enum {
  MeshTopologyBoundboxStatus_Success,
  MeshTopologyBoundboxStatus_AllocFail,
  MeshTopologyBoundboxStatus_UndefError,
} MeshTopologyBoundboxStatus;

typedef struct {
  VertexIndex index;
  VertexAttribute attribute;
  vec3 corners[8];
  AABB world;
  AABB local;
} MeshTopologyBoundbox;

void mesh_topology_boundbox_compute_bound(const MeshTopologyBase *, mat4,
                                          MeshTopologyBoundbox *);

MeshTopologyBoundboxStatus
mesh_topology_boundbox_create(MeshTopologyBase *, mat4, MeshTopologyBoundbox *);

MeshTopology mesh_topology_boundbox_vertex(MeshTopologyBoundbox *);

MeshTopologyBoundboxStatus
mesh_topology_boundbox_update(const MeshTopologyBase *, mat4,
                              MeshTopologyBoundbox *);

#endif
