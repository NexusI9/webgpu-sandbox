#ifndef _GIZMO_TRANSFORM_CALLBACK_H_
#define _GIZMO_TRANSFORM_CALLBACK_H_

#include "core.h"

void gizmo_transform_callback_translate(GizmoTransform *, Camera *, Viewport *,
                                        vec3 *);

void gizmo_transform_callback_rotate(GizmoTransform *, Camera *, Viewport *,
                                     vec3 *);

void gizmo_transform_callback_scale(GizmoTransform *, Camera *, Viewport *,
                                    vec3 *);

#endif
