#ifndef _SCENE_CREATE_H_
#define _SCENE_CREATE_H_

#include "../gizmo/gizmo.h"
#include "core.h"

// light
GizmoPointLight *scene_add_point_light(Scene *, PointLightDescriptor *);
GizmoSpotLight *scene_add_spot_light(Scene *, SpotLightDescriptor *);
GizmoAmbientLight *scene_add_ambient_light(Scene *, AmbientLightDescriptor *);
GizmoSunLight *scene_add_sun_light(Scene *, SunLightDescriptor *);

// camera
GizmoCamera *scene_add_camera(Scene *, const CameraCreateDescriptor *);

// mesh
Mesh *scene_new_mesh(Scene *);
void scene_add_mesh(Scene *, Mesh *, const ScenePipeline, const char *);
void scene_add_mesh_reference_list(Scene *, MeshRefList *, const ScenePipeline,
                                   const char *);
void scene_remove_mesh(Scene *, Mesh *);

#endif
