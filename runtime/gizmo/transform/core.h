#ifndef _GIZMO_TRANSFORM_CORE_H_
#define _GIZMO_TRANSFORM_CORE_H_

#include "../../mesh/mesh.h"
#include "../core.h"

typedef struct GizmoTransform GizmoTransform;

typedef void (*gizmo_transform_callback)(GizmoTransform *, Mesh *, int, int);

typedef enum {
  GizmoTransformMode_Translate = 0,
  GizmoTransformMode_Rotate = 1,
  GizmoTransformMode_Scale = 2,
} GizmoTransformMode;

struct GizmoTransform {
  MeshRefList target; // DELETEME ??
  GizmoTransformMode mode;
  MeshRefList *active_handle;
  MeshRefList handles[3];
  gizmo_transform_callback transform_callback[3];
};

void gizmo_transform_create(GizmoTransform *,
                            const GizmoCreateDescriptor *desc);

void gizmo_transform_update_mode(GizmoTransform *, MeshRefList *,
                                 GizmoTransformMode);

void gizmo_transform_remove(GizmoTransform *, MeshRefList *);

void gizmo_transform_translate(GizmoTransform *, vec3);
void gizmo_transform_rotate(GizmoTransform *, vec3);


#endif
