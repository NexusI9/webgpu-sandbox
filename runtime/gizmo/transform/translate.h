#ifndef _GIZMO_TRANSLATE_H_
#define _GIZMO_TRANSLATE_H_

#include "../../mesh/mesh.h"
#include "../core.h"

void gizmo_transform_translate_create(MeshRefList *,
                                           const GizmoCreateDescriptor *);

void gizmo_transform_translate_translate(MeshRefList *, vec3);
void gizmo_transform_translate_rotate(MeshRefList *, vec3);

#endif
