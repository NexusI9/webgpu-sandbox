#include "list.h"
#include "../utils/system.h"
#include "core.h"
#include "string.h"

static CameraStatus camera_list_expand(CameraList *);

/**
   Init camera list
 */
CameraStatus camera_list_create(CameraList *list, size_t capacity) {

  list->entries = malloc(capacity * sizeof(Camera));
  list->length = 0;

  if (list->entries == NULL) {
    VERBOSE_ERROR("Couldn't create new camera list.");
    return CameraStatus_AllocFail;
  }

  list->capacity = capacity;

  return CameraStatus_Success;
}

CameraStatus camera_list_expand(CameraList *list) {

  size_t new_capacity = list->capacity * 2;
  Camera *temp =
      (Camera *)realloc(list->entries, new_capacity * sizeof(Camera));

  if (temp == NULL) {
    VERBOSE_ERROR("Couldn't expand Camera list.");
    return CameraStatus_AllocFail;
  }

  list->entries = temp;
  list->capacity = new_capacity;

  return CameraStatus_Success;
}

/**
   Insert existing camera in the list
 */
Camera *camera_list_insert(CameraList *list, Camera *camera) {

  // check if list is init
  if (list->entries == NULL) {
    VERBOSE_ERROR("Camera list not initialized yet.");
    return NULL;
  }

  // check list capacity
  if (list->length == list->capacity &&
      camera_list_expand(list) != CameraStatus_Success)
    return NULL;

  // add new entry
  Camera *entry = &list->entries[list->length++];
  memcpy(entry, camera, sizeof(Camera));

  return entry;
}

Camera *camera_list_new_camera(CameraList *list) {

  // check if list is init
  if (list->entries == NULL) {
    VERBOSE_ERROR("Camera list not initialized yet.");
    return NULL;
  }

  // check list capacity
  if (list->length == list->capacity &&
      camera_list_expand(list) != CameraStatus_Success)
    return NULL;

  return &list->entries[list->length++];
}
