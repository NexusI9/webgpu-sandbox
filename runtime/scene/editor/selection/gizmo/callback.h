#ifndef _GIZMO_CALLBACK_H_
#define _GIZMO_CALLBACK_H_

#include <cglm/types.h>

#include "core.h"
#include "../runtime/camera/core.h"
#include "../runtime/viewport/core.h"

void gizmo_callback_position(Gizmo *, Camera *, Viewport *, vec3 *);

void gizmo_callback_rotation(Gizmo *, Camera *, Viewport *, vec3 *);

void gizmo_callback_scale(Gizmo *, Camera *, Viewport *, vec3 *);

#endif
