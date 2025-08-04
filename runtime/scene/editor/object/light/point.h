#ifndef _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_

#include "../core.h"
#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"

void seo_light_point_create(SceneEditorObject *, PointLight *,
                            const GizmoCreateDescriptor *);

void seo_light_point_translate(SceneEditorObject *, vec3);
void seo_light_point_rotate(SceneEditorObject *, vec3);
void seo_light_point_scale(SceneEditorObject *, vec3);

#endif
