#ifndef _SCENE_EDITOR_CALLBACK_HIGHLIGHT_H_
#define _SCENE_EDITOR_CALLBACK_HIGHLIGHT_H_

#include <cglm/cglm.h>

#include "runtime/mesh/list.h"
#include "runtime/scene/core.h"

void scene_selection_mesh_highlight(MeshRefList *, SceneSelectionObjectList *,
                                     void *);

void scene_selection_seo_highlight(MeshRefList *, SceneSelectionObjectList *,
                                    void *);

#endif
