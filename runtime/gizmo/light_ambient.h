#ifndef _GIZMO_AMBIENT_LIGHT_H_
#define _GIZMO_AMBIENT_LIGHT_H_

#include "../light.h"
#include "../mesh.h"
#include "./gizmo.h"

typedef struct {
  AmbientLight *target;
  MeshRefList meshes;
} GizmoAmbientLight;

void gizmo_light_ambient_create(GizmoAmbientLight *, AmbientLight *,
                                const GizmoCreateDescriptor *);
#endif
