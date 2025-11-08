#ifndef _GIZMO_SYSTEM_H_
#define _GIZMO_SYSTEM_H_

#include "backend/renderer/core.h"
#include "runtime/gizmo/core.h"
#include "runtime/scene/core.h"
#include "utils/defines.h"

EXTERN_C_BEGIN

void gizmo_system_show(Gizmo *, Renderer *);
void gizmo_system_hide(Gizmo *, Renderer *);

EXTERN_C_END
#endif
