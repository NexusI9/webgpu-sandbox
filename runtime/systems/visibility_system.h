#ifndef _VISBILITY_SYSTEM_H_
#define _VISBILITY_SYSTEM_H_

#include "backend/renderer/batch.h"
#include "backend/renderer/core.h"
#include "runtime/scene/core.h"
#include "utils/defines.h"

EXTERN_C_BEGIN

void visibility_system_show_mesh(Scene *, Renderer *, Mesh *);
void visibility_system_hide_mesh(Scene *, Renderer *, Mesh *);

void visibility_system_show_mesh_ref_list(Scene *, Renderer *, MeshRefList *);
void visibility_system_hide_mesh_ref_list(Scene *, Renderer *, MeshRefList *);

void visibility_system_toggle_mesh(Scene *, Renderer *, Mesh *);


EXTERN_C_END

#endif
