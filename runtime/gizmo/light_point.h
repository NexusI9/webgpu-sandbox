#ifndef _GIZMO_POINT_LIGHT_H_
#define _GIZMO_POINT_LIGHT_H_

#include "../light.h"
#include "../mesh.h"
#include "./gizmo.h"

typedef struct {
  PointLight *target;
  MeshRefList meshes;
} GizmoPointLight;

void gizmo_light_point_create(GizmoPointLight *, PointLight *,
                              const GizmoCreateDescriptor *);
#endif
