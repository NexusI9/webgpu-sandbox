#ifndef _SCENE_EVENT_CORE_H_
#define _SCENE_EVENT_CORE_H_

typedef enum {

  // scene events
  SceneEventType_None,

  // mesh events
  SceneEventType_MeshShown,
  SceneEventType_MeshHidden,
  SceneEventType_MeshBuilt,

  SceneEventType_MeshTransformStart,
  SceneEventType_MeshTransformUpdated,
  SceneEventType_MeshTransformCommited,
  SceneEventType_MeshTransformCanceled,

  SceneEventType_MeshAdded,
  SceneEventType_MeshRemoved,

  // camera
  SceneEventType_CameraTransformStart,
  SceneEventType_CameraTransformUpdated,
  SceneEventType_CameraTransformCommited,
  SceneEventType_CameraTransformCanceled,

  SceneEventType_CameraAdded,
  SceneEventType_CameraRemoved,

  // light
  SceneEventType_LightTransformStart,
  SceneEventType_LightTransformUpdated,
  SceneEventType_LightTransformCommited,
  SceneEventType_LightTransformCanceled,

  SceneEventType_LightAdded,
  SceneEventType_LightRemoved,

  // selection events
  SceneEventType_SelectionUpdated,
  SceneEventType_SelectionCleared,

} SceneEventType;

typedef void (*SceneEventDispatcher)(const SceneEventType, void *);

void scene_event_dispatcher(const SceneEventType, void *);
#endif
