#ifndef _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_GRID_H_
#define _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_GRID_H_

#include "../runtime/probe/probe.h"
#include "../runtime/scene/core.h"

void seo_probe_reflection_grid_create(SceneEditorObject *, ProbeReflectionGrid *,
                                 const SEOCreateDescriptor *);

void seo_probe_reflection_grid_bound_set_position(SEOTransformCallback *);
void seo_probe_reflection_grid_bound_set_scale(SEOTransformCallback *);

void seo_probe_reflection_grid_set_position(SEOTransformCallback *);
void seo_probe_reflection_grid_set_rotation(SEOTransformCallback *);
void seo_probe_reflection_grid_set_scale(SEOTransformCallback *);

#endif
