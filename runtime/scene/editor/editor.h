#ifndef _SCENE_EDITOR_H_
#define _SCENE_EDITOR_H_

#include "backend/ubo.h"
#include "mesh/list/list.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/scene/core.h"

/* DELETEME
typedef struct {

  MeshList *mesh_list;
  SceneEditorMeshList *sem_list; // cam/ lights  lists

  MeshRefList *fixed_pipeline;
  MeshRefList *selection_pipeline;

  Camera *active_camera;
  Viewport *viewport;

  UBOManager *ubo;

  // selection sets
  SceneSelection selection;

  struct {
    Gizmo transform; // transform gizmo (unique)
    Mesh *grid;      // grid gizmo (unique)
  } gizmo;

} SceneEditorDescriptor;
*/

void scene_editor_init(Scene *);

/**
   Return the scene editor gizmo list.
   Used when adding lights or camera into the scene.
 */
static inline SceneEditorMeshListArray *
scene_editor_mesh_list(SceneEditor *editor) {
  return &editor->sem_list;
}

#endif
