#ifndef _SCENE_EDITOR_OBJECT_AMBIENT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_AMBIENT_LIGHT_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include "../core.h"

void seo_light_ambient_create(SceneEditorObject *, AmbientLight *,
                                const GizmoCreateDescriptor *);
#endif
