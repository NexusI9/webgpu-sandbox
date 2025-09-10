#ifndef _SCENE_EDITOR_CALLBACK_TRANSFORM_H_
#define _SCENE_EDITOR_CALLBACK_TRANSFORM_H_

#include "../../../core.h"
#include <cglm/cglm.h>

// mesh transform
void scene_selection_mesh_transform(SceneSelectionTransform *);

// seo transform
void scene_selection_seo_transform(SceneSelectionTransform *);

// mesh shadow transform
void scene_selection_mesh_shadow_transform(SceneSelectionTransform *);

// TODO: move this function to a more appropriate space and rename it
// accordingly. (This process isn't exclusive to selection...)
void scene_selection_mesh_update_probe_uniform(Mesh *,
                                               ProbeReflectionGridList *,
                                               ProbeReflectionPlaneList *,
                                               SSBOManager *);
#endif
