#ifndef _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/scene/core.h"

void seo_light_spot_create(SceneEditorObject*, SpotLight *, const SEOCreateDescriptor *);


void seo_light_spot_translate(SceneEditorObject *, vec3);
void seo_light_spot_rotate(SceneEditorObject *, vec3);
void seo_light_spot_scale(SceneEditorObject *, vec3);


#endif
