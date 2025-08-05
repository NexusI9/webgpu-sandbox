#ifndef _SCENE_EDITOR_CAMERA_H_
#define _SCENE_EDITOR_CAMERA_H_

#include "../runtime/scene/core.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/mesh.h"

void seo_camera_create(SceneEditorObject *, Camera *,
                       const SEOCreateDescriptor *);

void seo_camera_translate(SceneEditorObject *, vec3);
void seo_camera_rotate(SceneEditorObject *, vec3);
void seo_camera_scale(SceneEditorObject *, vec3);

void seo_camera_lookat(SceneEditorObject *, vec3, vec3);
void seo_camera_fov(SceneEditorObject *, float);

#endif
