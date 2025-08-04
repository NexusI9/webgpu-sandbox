#ifndef _SCENE_EDTIOR_OBJECT_LIST_H_
#define _SCENE_EDTIOR_OBJECT_LIST_H_

#include "../core.h"

#define SCENE_EDITOR_OBJECT_LIST_CAPACITY_DEFAULT 16

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

typedef struct {
  size_t length;
  size_t capacity;
  SceneEditorObject *entries;
} SceneEditorObjectList;

DynamicListStatus seo_list_create(SceneEditorObjectList *, size_t);

SceneEditorObject *seo_list_insert(SceneEditorObjectList *, SceneEditorObject *);

SceneEditorObject *seo_list_new_entry(SceneEditorObjectList *);

#endif
