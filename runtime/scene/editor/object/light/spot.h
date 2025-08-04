#ifndef _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_
#define _SCENE_EDITOR_OBJECT_SPOT_LIGHT_H_

#include "../runtime/light/light.h"
#include "../runtime/mesh/mesh.h"
#include "../core.h"

void seo_light_spot_create(SceneEditorObject*, SpotLight *, const GizmoCreateDescriptor *);

#endif
