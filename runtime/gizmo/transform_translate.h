#ifndef _GIZMO_TRANSLATE_H_
#define _GIZMO_TRANSLATE_H_

#include "../runtime/mesh/mesh.h"
#include "core.h"

typedef struct {
  MeshRefList target;
  MeshRefList meshes;
} GizmoTransformTranslate;

void gizmo_transform_translate_create(GizmoTransformTranslate *,
                                      const GizmoCreateDescriptor *);
void gizmo_transform_translate_translate(GizmoTransformTranslate *, vec3);
void gizmo_transform_translate_rotate(GizmoTransformTranslate *, vec3);

#endif
