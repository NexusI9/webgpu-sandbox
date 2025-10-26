#ifndef _SCENE_EDITOR_SELECTION_UTILS_H_
#define _SCENE_EDITOR_SELECTION_UTILS_H_

#include "backend/ubo.h"
#include "gizmo/core.h"
#include "runtime/scene/core.h"

EXTERN_C_BEGIN

/* Gizmo utils */
void scene_gizmo_pos_to_selection(Gizmo *, SceneSelection *, UBOManager *);
void scene_gizmo_show(Scene *);
void scene_gizmo_hide(Scene *);

EXTERN_C_END

#endif
