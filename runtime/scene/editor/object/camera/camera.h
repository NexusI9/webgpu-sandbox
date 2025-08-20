#ifndef _SCENE_EDITOR_CAMERA_H_
#define _SCENE_EDITOR_CAMERA_H_

#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/scene/core.h"

void seo_camera_create(SceneEditorObject *, Camera *,
                       const SEOCreateDescriptor *);

void seo_camera_set_position(Mesh*, SceneEditorObject *, vec3);
void seo_camera_set_rotation(Mesh*, SceneEditorObject *, vec3);
void seo_camera_set_scale(Mesh*, SceneEditorObject *, vec3);

void seo_camera_lookat(SceneEditorObject *, vec3, vec3);
void seo_camera_fov(SceneEditorObject *, float);

#endif
