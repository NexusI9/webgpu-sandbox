#ifndef _GIZMO_TRANSFORM_CALLBACK_H_
#define _GIZMO_TRANSFORM_CALLBACK_H_

#include "core.h"

void gizmo_transform_callback_translate(GizmoTransform *, MeshRefList *,
                                        Camera *);
void gizmo_transform_callback_rotate(GizmoTransform *, MeshRefList *, Camera *);
void gizmo_transform_callback_scale(GizmoTransform *, MeshRefList *, Camera *);

#endif
