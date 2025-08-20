#ifndef _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_H_
#define _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_H_

#include "../runtime/probe/probe.h"
#include "../runtime/scene/core.h"

void seo_probe_reflection_create(SceneEditorObject *, ProbeReflectionGrid *,
                                 const SEOCreateDescriptor *);

void seo_probe_reflection_bound_set_position(SEOTransformCallback *);
void seo_probe_reflection_bound_set_scale(SEOTransformCallback *);

void seo_probe_reflection_set_position(SEOTransformCallback *);
void seo_probe_reflection_set_rotation(SEOTransformCallback *);
void seo_probe_reflection_set_scale(SEOTransformCallback *);

#endif
