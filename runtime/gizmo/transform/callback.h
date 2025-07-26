#ifndef _GIZMO_TRANSFORM_CALLBACK_H_
#define _GIZMO_TRANSFORM_CALLBACK_H_

#include "core.h"

void gizmo_transform_callback_translate(GizmoTransform *, Camera *, Viewport *);

void gizmo_transform_callback_rotate(GizmoTransform *, Camera *, Viewport *);

void gizmo_transform_callback_scale(GizmoTransform *, Camera *, Viewport *);

#endif
