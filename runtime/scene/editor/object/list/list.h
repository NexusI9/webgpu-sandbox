#ifndef _SCENE_EDTIOR_OBJECT_LIST_H_
#define _SCENE_EDTIOR_OBJECT_LIST_H_

#include "../runtime/scene/scene.h"

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


DynamicListStatus seo_list_create(SceneEditorObjectList *, size_t);

SceneEditorObject *seo_list_insert(SceneEditorObjectList *, SceneEditorObject *);

SceneEditorObject *seo_list_new_entry(SceneEditorObjectList *);

#endif
