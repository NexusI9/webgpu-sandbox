#ifndef _MESH_TOPOLOGY_BASE_H_
#define _MESH_TOPOLOGY_BASE_H_
#include <cglm/types.h>
#include <webgpu/webgpu.h>

#include "../../geometry/vertex/vertex.h"
#include "anchor.h"
#include "core.h"
#include "../runtime/geometry/vertex/attribute.h"
#include "../runtime/geometry/vertex/group.h"
#include "../runtime/geometry/vertex/index.h"
#include "../runtime/geometry/vertex/attribute.h"
#include "../runtime/geometry/vertex/group.h"
#include "../runtime/geometry/vertex/index.h"

typedef enum {
  MeshTopologyBaseStatus_Success,
  MeshTopologyBaseStatus_AllocFail,
  MeshTopologyBaseStatus_Empty,
  MeshTopologyBaseStatus_UndefError,
} MeshTopologyBaseStatus;

typedef struct {
  VertexAttribute attribute;
  VertexIndex index;
  VertexGroupSet group;
  MeshTopologyAnchorList siblings;
} MeshTopologyBase;

MeshTopology mesh_topology_base_vertex(MeshTopologyBase *);

void mesh_topology_base_create(MeshTopologyBase *, const VertexAttribute *,
                               const VertexIndex *, const WGPUDevice,
                               const WGPUQueue);

MeshTopologyBaseStatus
mesh_topology_base_create_vertex_attribute(MeshTopologyBase *,
                                           const VertexAttribute *,
                                           const WGPUDevice, const WGPUQueue);

MeshTopologyBaseStatus
mesh_topology_base_create_vertex_index(MeshTopologyBase *, const VertexIndex *,
                                       const WGPUDevice, const WGPUQueue);

void mesh_topology_base_set_scale(MeshTopologyBase *, const VertexGroup *, vec3 *);

void mesh_topology_base_set_position(MeshTopologyBase *, const VertexGroup *,
                                  vec3 *);

void mesh_topology_base_update_buffer(MeshTopologyBase *, const WGPUDevice,
                                      const WGPUQueue);
#endif
