#ifndef _GIZMO_TRANSFORM_CORE_H_
#define _GIZMO_TRANSFORM_CORE_H_

#include "../../mesh/mesh.h"
#include "../core.h"

typedef enum {
  GizmoTransformMode_Translate = 0,
  GizmoTransformMode_Rotate = 1,
  GizmoTransformMode_Scale = 2,
} GizmoTransformMode;

typedef struct {
  MeshRefList *target;
  MeshRefList meshes;
} GizmoTransformHandle;

typedef struct {
  MeshRefList target;
  GizmoTransformMode mode;
  MeshRefList *active_handle;
  MeshRefList handles[3];
} GizmoTransform;

void gizmo_transform_create(GizmoTransform *,
                            const GizmoCreateDescriptor *desc);

void gizmo_transform_update_mode(GizmoTransform *, MeshRefList *,
                                 GizmoTransformMode);

void gizmo_transform_remove(GizmoTransform *, MeshRefList *);

void gizmo_transform_translate(GizmoTransform *, vec3);
void gizmo_transform_rotate(GizmoTransform *, vec3);

#endif
