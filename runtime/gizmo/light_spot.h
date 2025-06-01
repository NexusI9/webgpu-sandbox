#ifndef _GIZMO_SPOT_LIGHT_H_
#define _GIZMO_SPOT_LIGHT_H_

#include "../light.h"
#include "../mesh.h"
#include "./light.h"

typedef struct {
  SpotLight *target;
  MeshRefList meshes;
} GizmoSpotLight;

void gizmo_light_spot_create(SpotLight *, const GizmoLightCreateDescriptor *);

#endif
