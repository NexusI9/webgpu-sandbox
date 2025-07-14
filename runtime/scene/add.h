#ifndef _SCENE_ADD_H_
#define _SCENE_ADD_H_

#include "../gizmo/gizmo.h"
#include "core.h"

// light
GizmoPointLight *scene_add_point_light(Scene *, PointLightDescriptor *);
GizmoSpotLight *scene_add_spot_light(Scene *, SpotLightDescriptor *);
GizmoAmbientLight *scene_add_ambient_light(Scene *, AmbientLightDescriptor *);
GizmoSunLight *scene_add_sun_light(Scene *, SunLightDescriptor *);

// camera
GizmoCamera *scene_add_camera(Scene *, const CameraCreateDescriptor *);

#endif
