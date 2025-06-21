#include "list.h"
#include "string.h"

static int camera_list_expand(CameraList *);

/**
   Init camera list
 */
int camera_list_create(CameraList *list, size_t capacity) {

  list->entries = malloc(capacity * sizeof(Camera));
  list->length = 0;

  if (list->entries == NULL) {
    perror("Couldn't create new camera list\n");
    return CAMERA_ALLOC_FAIL;
  }

  list->capacity = capacity;

  return CAMERA_SUCCESS;
}

int camera_list_expand(CameraList *list) {

  size_t new_capacity = list->capacity * 2;
  Camera *temp =
      (Camera *)realloc(list->entries, new_capacity * sizeof(Camera));

  if (temp == NULL) {
    perror("Couldn't expand Camera list\n");
    return CAMERA_ALLOC_FAIL;
  }

  list->entries = temp;
  list->capacity = new_capacity;

  return CAMERA_SUCCESS;
}

/**
   Insert existing camera in the list
 */
Camera *camera_list_insert(CameraList *list, Camera *camera) {

  // check if list is init
  if (list->entries == NULL) {
    perror("Camera list not initialized yet\n");
    return NULL;
  }

  // check list capacity
  if (list->length == list->capacity &&
      camera_list_expand(list) != CAMERA_SUCCESS)
    return NULL;

  // add new entry
  Camera *entry = &list->entries[list->length++];
  memcpy(entry, camera, sizeof(Camera));

  return entry;
}

Camera *camera_list_new_camera(CameraList *list) {

  // check if list is init
  if (list->entries == NULL) {
    perror("Camera list not initialized yet\n");
    return NULL;
  }

  // check list capacity
  if (list->length == list->capacity &&
      camera_list_expand(list) != CAMERA_SUCCESS)
    return NULL;

  return &list->entries[list->length++];
}
