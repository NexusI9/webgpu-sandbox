#ifndef _GIZMO_TRANSFORM_UTILS_H_
#define _GIZMO_TRANSFORM_UTILS_H_

#include "../../mesh/mesh.h"
#include "../utils/color.h"
#include "core.h"

typedef struct {
  const char *mbin_path;
  const WGPUDevice *device;
  const WGPUQueue *queue;
  MeshList *list;
} GizmoTransformCreateMeshDescriptor;


void gizmo_transform_create_mesh(Mesh *, Primitive *, const color *,
                                 const WGPUQueue *, const WGPUDevice *);

void gizmo_transform_create_handles(MeshRefList *, GizmoTransformMeshAxis *,
                                    const GizmoTransformCreateMeshDescriptor *);

void gizmo_transform_origin(GizmoTransform *, vec3 *);

#endif
