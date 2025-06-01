#ifndef _GIZMO_SUN_LIGHT_H_
#define _GIZMO_SUN_LIGHT_H_

#include "../light.h"
#include "../mesh.h"
#include "./light.h"

typedef struct {
  SunLight *target;
  MeshRefList meshes;
} GizmoSunLight;

void gizmo_light_sun_create(SunLight *, const GizmoLightCreateDescriptor *);

#endif
