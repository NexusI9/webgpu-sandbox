#ifndef _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_PLANE_H_
#define _SCENE_EDITOR_OBJECT_PROBE_REFLECTION_PLANE_H_

#include "../runtime/probe/probe.h"
#include "../runtime/scene/core.h"
#include "../runtime/probe/reflection/plane.h"

void seo_probe_reflection_plane_create(SceneEditorObject *,
                                       ProbeReflectionPlane *,
                                       const SEOCreateDescriptor *);

void seo_probe_reflection_plane_set_position(SEOTransformCallback *);
void seo_probe_reflection_plane_set_rotation(SEOTransformCallback *);
void seo_probe_reflection_plane_set_scale(SEOTransformCallback *);
void seo_probe_reflection_plane_update_mesh_uniform(SceneEditorObject *);

#endif
