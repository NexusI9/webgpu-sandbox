#ifndef _GIZMO_SUN_LIGHT_H_
#define _GIZMO_SUN_LIGHT_H_

#include "../light.h"
#include "../mesh/mesh.h"
#include "./core.h"

typedef struct {
  SunLight *target;
  MeshRefList meshes;
} GizmoSunLight;

void gizmo_light_sun_create(GizmoSunLight*, SunLight *, const GizmoCreateDescriptor *);

#endif
