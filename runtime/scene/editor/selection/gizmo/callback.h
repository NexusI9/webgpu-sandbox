#ifndef _GIZMO_CALLBACK_H_
#define _GIZMO_CALLBACK_H_

#include "core.h"

void gizmo_callback_position(Gizmo *, Camera *, Viewport *, vec3 *);

void gizmo_callback_rotation(Gizmo *, Camera *, Viewport *, vec3 *);

void gizmo_callback_scale(Gizmo *, Camera *, Viewport *, vec3 *);

#endif
