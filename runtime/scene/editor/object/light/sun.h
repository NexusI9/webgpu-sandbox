#ifndef _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_

#include "../core.h"
#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"

void seo_light_sun_create(SceneEditorObject *, SunLight *,
                            const GizmoCreateDescriptor *);


void seo_light_sun_translate(SceneEditorObject *, vec3);
void seo_light_sun_rotate(SceneEditorObject *, vec3);
void seo_light_sun_scale(SceneEditorObject *, vec3);


#endif
