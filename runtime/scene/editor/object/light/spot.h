#ifndef _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include "../runtime/scene/core.h"
#include "../runtime/light/core.h"
#include "../runtime/light/list.h"

/* Base */
void seo_light_spot_create(SceneEditorObject *, SpotLight *,
                           const SEOCreateDescriptor *);

void seo_light_spot_set_position(SEOTransformCallback *);
void seo_light_spot_set_rotation(SEOTransformCallback *);
void seo_light_spot_set_scale(SEOTransformCallback *);

void seo_light_spot_update_transform_callback(SceneEditorObject *,
                                              const LightShadow);

/* Shadow */

void seo_light_spot_shadow_create(SceneEditorObject *, SpotLight *,
                                  const SEOCreateDescriptor *);

void seo_light_spot_shadow_set_position(SEOTransformCallback *);
void seo_light_spot_shadow_set_rotation(SEOTransformCallback *);

#endif
