#ifndef _SCENE_EDITOR_CALLBACK_TRANSFORM_H_
#define _SCENE_EDITOR_CALLBACK_TRANSFORM_H_

#include "../../../core.h"
#include <cglm/cglm.h>

void scene_selection_mesh_transform(MeshRefList *, SceneSelectionTargetList *,
                                    Vec3List *, vec3, const Axis,
                                    const GizmoTransformMode, void *);

void scene_selection_seo_transform(MeshRefList *, SceneSelectionTargetList *,
                                   Vec3List *, vec3, const Axis,
                                   const GizmoTransformMode, void *);

#endif
