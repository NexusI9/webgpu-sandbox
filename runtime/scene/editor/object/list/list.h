#ifndef _SCENE_EDTIOR_OBJECT_LIST_H_
#define _SCENE_EDTIOR_OBJECT_LIST_H_

#include <stddef.h>

#include "runtime/scene/scene.h"
#include "runtime/scene/core.h"
#include "utils/dyli.h"

/*
  GIZMO LIST
  Gizmos' scene entities (meshes) are spearated from their data (primary
  struct). As a result gizmos meshes and data are gathered under the
  GizmoList struct.


      Mesh Pool
     .----------.
     |  Mesh 1  | -----------.
     |----------|            |      Gizmo List
     |  Mesh 2  | -----------|   .-------------.
     |----------|            '-> |  meshes [*] |
     |  Mesh n  |                |  length 2   |
     '----------'                |  -          |
                             .-> |  target *   |
      Gizmo Pool            |    '-------------'
     .-----------.          |
     | Camera 1  | ---------'
     |-----------|
     | Camera n  |
     '-----------'

 */

/* === SEO LIST === */
DynamicListStatus seo_list_create(SceneEditorObjectList *, size_t);

SceneEditorObject *seo_list_insert(SceneEditorObjectList *,
                                   SceneEditorObject *);

SceneEditorObject *seo_list_new_entry(SceneEditorObjectList *);

DynamicListStatus seo_list_remove(SceneEditorObjectList *, SceneEditorObject *);

DynamicListStatus seo_list_destroy(SceneEditorObjectList *);

/* === SEO MESH === */

DynamicListStatus seo_mesh_list_create(SceneEditorObjectMeshList *, size_t);

DynamicListStatus seo_mesh_list_insert(SceneEditorObjectMeshList *,
                                        SceneEditorObjectMesh *);

SceneEditorObjectMesh *seo_mesh_list_new_entry(SceneEditorObjectMeshList *);

DynamicListStatus seo_mesh_list_remove(SceneEditorObjectMeshList *,
                                       SceneEditorObjectMesh *);

DynamicListStatus seo_mesh_list_destroy(SceneEditorObjectMeshList *);

#endif
