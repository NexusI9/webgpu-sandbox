#ifndef _SCENE_ADD_H_
#define _SCENE_ADD_H_

#include "../gizmo/gizmo.h"
#include "core.h"

// light
GizmoPointLight *scene_add_point_light(Scene *, PointLightDescriptor *, WGPUDevice *,
                                  WGPUQueue *);
GizmoSpotLight *scene_add_spot_light(Scene *, SpotLightDescriptor *, WGPUDevice *,
                                WGPUQueue *);
GizmoAmbientLight *scene_add_ambient_light(Scene *, AmbientLightDescriptor *,
                                      WGPUDevice *, WGPUQueue *);
GizmoSunLight *scene_add_sun_light(Scene *, SunLightDescriptor *, WGPUDevice *,
                              WGPUQueue *);

// camera
GizmoCamera *scene_add_camera(Scene *, const CameraCreateDescriptor *, WGPUDevice *,
                         WGPUQueue *);


#endif
