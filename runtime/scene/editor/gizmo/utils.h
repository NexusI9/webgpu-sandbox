#ifndef _GIZMO_TRANSFORM_UTILS_H_
#define _GIZMO_TRANSFORM_UTILS_H_

#include "../runtime/mesh/mesh.h"
#include "../utils/color.h"
#include "core.h"

typedef struct {
  const char *mbin_path;
  const WGPUDevice device;
  const WGPUQueue queue;
  const Pipeline *pipeline;
  MeshList *list;
} GizmoTransformCreateMeshDescriptor;

void gizmo_transform_create_mesh(Mesh *, const Pipeline *, Primitive *,
                                 const color *, const WGPUQueue,
                                 const WGPUDevice);

void gizmo_transform_create_handles(MeshRefList *, MeshRefList *,
                                    const GizmoTransformCreateMeshDescriptor *);

void gizmo_transform_origin(GizmoTransform *, vec3 *);

#endif
