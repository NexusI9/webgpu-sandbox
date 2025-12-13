#include "list.h"

#include "backend/logger.h"
#include "backend/resource_manager.h"
#include "core.h"
#include "utils/dyli.h"

/**
   Init camera list
 */
DynamicListStatus camera_list_create(CameraList *list, size_t capacity) {

  return dyli_create((void *)&list->entries, &list->capacity, &list->count,
                     sizeof(Camera *), capacity, "Camera list");
}

/**
   Insert existing camera in the list
 */
DynamicListStatus camera_list_insert(CameraList *list, Camera *camera) {
  return dyli_insert((void *)&list->entries, &list->capacity, &list->count,
                     sizeof(Camera *), (void *)&camera, 1, "Camera list");
}

Camera *camera_list_new_camera(CameraList *list) {

  Camera *cam = rem_new_camera();

  if (cam == NULL) {
    logger_add(LoggerFlag_Error,
               "Couldn't create new camera. Max capacity reached.");
    return NULL;
  }

  if (camera_list_insert(list, cam) != DynamicListStatus_Success) {
    logger_add(LoggerFlag_Error,
               "Couldn't insert newly created camera to the list.");
    return NULL;
  }

  return cam;
}
