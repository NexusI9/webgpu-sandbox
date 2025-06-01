#ifndef _GIZMO_SPOT_LIGHT_H_
#define _GIZMO_SPOT_LIGHT_H_

#include "../light/light.h"
#include "../mesh/mesh.h"
#include "./core.h"

typedef struct {
  SpotLight *target;
  MeshRefList meshes;
} GizmoSpotLight;

void gizmo_light_spot_create(GizmoSpotLight*, SpotLight *, const GizmoCreateDescriptor *);

#endif
