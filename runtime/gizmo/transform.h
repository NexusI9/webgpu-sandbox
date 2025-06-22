#ifndef _GIZMO_TRANSFORM_H_
#define _GIZMO_TRANSFORM_H_
#include "../mesh/mesh.h"

typedef struct {
  const char *mbin_path;
  const WGPUDevice *device;
  const WGPUQueue *queue;
  MeshList *list;
} GizmoTransformCreateMeshDescriptor;

void gizmo_transform_create_mesh(MeshRefList *,
                                 const GizmoTransformCreateMeshDescriptor *);

#endif
