#ifndef _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SUN_LIGHT_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/scene/core.h"

/* Base */
void seo_light_sun_create(SceneEditorObject *, SunLight *,
                          const SEOCreateDescriptor *);

void seo_light_sun_translate(SceneEditorObject *, vec3);
void seo_light_sun_rotate(SceneEditorObject *, vec3);
void seo_light_sun_scale(SceneEditorObject *, vec3);

void seo_light_sun_update_transform_callback(SceneEditorObject*, const LightShadow);

/* Shadow */
void seo_light_sun_shadow_create(SceneEditorObject *, SunLight *,
                                 const SEOCreateDescriptor *);

void seo_light_sun_shadow_translate(SceneEditorObject *, vec3);

#endif
