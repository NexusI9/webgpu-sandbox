#ifndef _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_H_
#define _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_H_

#include "../runtime/probe/probe.h"
#include "../runtime/scene/core.h"

void seo_probe_reflection_create(SceneEditorObject *, ProbeReflectionGrid *,
                                 const SEOCreateDescriptor *);

void seo_probe_reflection_translate(SceneEditorObject *, vec3);
void seo_probe_reflection_rotate(SceneEditorObject *, vec3);
void seo_probe_reflection_scale(SceneEditorObject *, vec3);

#endif
