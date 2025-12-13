#ifndef _SCENE_EDITOR_SELECTION_CORE_H_
#define _SCENE_EDITOR_SELECTION_CORE_H_

#include "backend/registry.h"

#include <cglm/types.h>
#include <stddef.h>

#include "emscripten/html5.h"
#include "runtime/gizmo/core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/selection/filter.h"

#define SCENE_SELECTION_LIST_CAPACITY 6

#define SCENE_SELECTION_TYPE_COUNT 3
typedef enum {
  SceneSelectionType_Mesh,
  SceneSelectionType_MeshShadow, // update shadow map on move
  SceneSelectionType_SEM,
} SceneSelectionType;

typedef struct {
  SceneSelectionFilter filters[SCENE_SELECTION_TYPE_COUNT];
} SceneSelection;

EXTERN_C_BEGIN

void scene_selection_init(SceneSelection *);

void scene_selection_average_position(SceneSelection *, vec3 *);

size_t scene_selection_count(SceneSelection *);

void scene_selection_register_mesh_ref_list(SceneSelection *, MeshRefList *,
                                            reg_id_t, const SceneSelectionType);

void scene_selection_register_mesh(SceneSelection *, Mesh *, reg_id_t,
                                   const SceneSelectionType);

void scene_selection_unregister_mesh(SceneSelection *, Mesh *,
                                     const SceneSelectionType);

SceneSelectionFilter *
scene_selection_find_filter_of_mesh(SceneSelection *, Mesh *, bool *, size_t *);

void scene_selection_empty(SceneSelection *);
void scene_selection_all(SceneSelection *);
void scene_selection_cache_initial_attributes(SceneSelection *,
                                              const GizmoMode);
void scene_selection_clear_initial_attributes(SceneSelection *);

static inline SceneSelectionFilter *
scene_selection_filter(SceneSelection *selection,
                       const SceneSelectionType type) {
  return &selection->filters[type];
}

EXTERN_C_END

#endif
