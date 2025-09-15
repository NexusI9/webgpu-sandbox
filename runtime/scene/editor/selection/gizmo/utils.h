#ifndef _GIZMO_UTILS_H_
#define _GIZMO_UTILS_H_

#include <cglm/types.h>
#include <webgpu/webgpu.h>

#include "runtime/mesh/mesh.h"
#include "utils/color.h"
#include "core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/primitive/core.h"

typedef struct {
  const char *mbin_path;
  const WGPUDevice device;
  const WGPUQueue queue;
  MeshList *list;
} GizmoCreateMeshDescriptor;

void gizmo_create_mesh(Mesh *, Primitive *, const color *, const WGPUQueue,
                       const WGPUDevice);

void gizmo_create_handles(MeshRefList *, MeshRefList *,
                          const GizmoCreateMeshDescriptor *);

void gizmo_origin(Gizmo *, vec3 *);

#endif
