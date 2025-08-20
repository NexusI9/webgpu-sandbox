#ifndef _SCENE_EDITOR_OBJECT_AMBIENT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_AMBIENT_LIGHT_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/scene/core.h"

void seo_light_ambient_create(SceneEditorObject *, AmbientLight *,
                              const SEOCreateDescriptor *);

void seo_light_ambient_set_position(SEOTransformCallback *);
void seo_light_ambient_set_rotation(SEOTransformCallback *);
void seo_light_ambient_set_scale(SEOTransformCallback *);

#endif
