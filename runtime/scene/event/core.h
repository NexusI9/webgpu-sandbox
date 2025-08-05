#ifndef _SCENE_EVENT_CORE_H_
#define _SCENE_EVENT_CORE_H_

typedef enum {

  // scene events
  SceneEventType_None = 0,

  // mesh events
  SceneEventType_MeshShown = 1 << 0,
  SceneEventType_MeshHidden = 1 << 1,
  SceneEventType_MeshBuilt = 1 << 2,

  // object events
  SceneEventType_ObjectAdded = 1 << 3,
  SceneEventType_ObjectRemoved = 1 << 4,

  // transform events
  SceneEventType_ObjectTransformStart = 1 << 5,
  SceneEventType_ObjectTransformUpdated = 1 << 6,
  SceneEventType_ObjectTransformCommited = 1 << 7,
  SceneEventType_ObjectTransformCanceled = 1 << 8,

  // selection events
  SceneEventType_SelectionUpdated = 1 << 9,
  SceneEventType_SelectionCleared = 1 << 10,

  // mouse events
  SceneEventType_MouseDown = 1 << 11,
  SceneEventType_MouseUp = 1 << 12,
  SceneEventType_MouseMove = 1 << 13,

  // key events
  SceneEventType_KeyDown = 1 << 14,
  SceneEventType_KeyUp = 1 << 15,
} SceneEventType;

#endif
