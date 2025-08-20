#ifndef _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_POINT_LIGHT_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/scene/core.h"

/* base */
void seo_light_point_create(SceneEditorObject *, PointLight *,
                            const SEOCreateDescriptor *);

void seo_light_point_set_position(Mesh *, SceneEditorObject *, vec3);
void seo_light_point_set_rotation(Mesh *, SceneEditorObject *, vec3);
void seo_light_point_set_scale(Mesh *, SceneEditorObject *, vec3);

void seo_light_point_update_transform_callback(SceneEditorObject *,
                                               const LightShadow);

/* shadow  */

void seo_light_point_shadow_create(SceneEditorObject *, PointLight *,
                                   const SEOCreateDescriptor *);

void seo_light_point_shadow_set_position(Mesh*, SceneEditorObject *, vec3);

#endif
