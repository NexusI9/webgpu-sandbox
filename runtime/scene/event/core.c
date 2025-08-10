#include "core.h"

// branches
#include "./event.mesh.h"
#include "./event.object.h"
#include "./event.selection.h"
#include "./event.camera.h"
#include "./event.light.h"

/**
   Global scene dispatcher, redispatch towards the branches based on lookup
   table.
 */
void scene_event_dispatcher(const SceneEventType type, void *sender) {

  switch (type) {

    // mesh
  case SceneEventType_MeshShown:
  case SceneEventType_MeshHidden:
  case SceneEventType_MeshBuilt:
    scene_event_dispatcher_mesh(type, sender);
    break;

    // selection
  case SceneEventType_SelectionUpdated:
  case SceneEventType_SelectionCleared:
    scene_event_dispatcher_selection(type, sender);
    break;

  case SceneEventType_None:
  default:
    break;
  }
}
