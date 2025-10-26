#ifndef _SCENE_EDITOR_CALLBACK_TRANSFORM_H_
#define _SCENE_EDITOR_CALLBACK_TRANSFORM_H_

#include <cglm/cglm.h>

#include "runtime/scene/core.h"
#include "backend/ubo.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"

// mesh transform
void scene_selection_mesh_transform(SceneSelectionTransform *);

// sem transform
void scene_selection_sem_transform(SceneSelectionTransform *);

// mesh shadow transform
void scene_selection_mesh_shadow_transform(SceneSelectionTransform *);

// TODO: move this function to a more appropriate space and rename it
// accordingly. (This process isn't exclusive to selection...)
void scene_selection_mesh_update_probe_uniform(Mesh *,
                                               ProbeReflectionGridList *,
                                               ProbeReflectionPlaneList *,
                                               UBOManager *);
#endif
