#ifndef _SCENE_EDITOR_H_
#define _SCENE_EDITOR_H_

#include "backend/ssbo.h"
#include "backend/ubo.h"
#include "object/list/list.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/scene/core.h"

/* DELETEME
typedef struct {

  WGPUDevice device;
  WGPUQueue queue;

  MeshList *mesh_list;
  SceneEditorObjectList *seo_list; // cam/ lights  lists

  MeshRefList *fixed_pipeline;
  MeshRefList *selection_pipeline;

  Camera *active_camera;
  Viewport *viewport;

  SSBOManager *ssbo;
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

SceneEditorObjectList *scene_editor_object_list(Scene *);

#endif
