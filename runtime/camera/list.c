#include "list.h"
#include "../utils/system.h"
#include "core.h"
#include "string.h"

/**
   Init camera list
 */
DynamicListStatus camera_list_create(CameraList *list, size_t capacity) {

  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(Camera), capacity, "Camera list");
}

/**
   Insert existing camera in the list
 */
DynamicListStatus camera_list_insert(CameraList *list, Camera *camera) {
  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(Camera), (void *)camera, 1, "Camera list");
}

Camera *camera_list_new_camera(CameraList *list) {
  return (Camera *)dyli_new_entry((void *)&list->entries, &list->capacity,
                                  &list->length, sizeof(Camera), "Camera list");
}
