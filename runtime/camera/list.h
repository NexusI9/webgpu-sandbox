#ifndef _CAMERA_LIST_H_
#define _CAMERA_LIST_H_

#include "../utils/dyli.h"
#include "core.h"

// camera list
DynamicListStatus camera_list_create(CameraList *, size_t);
DynamicListStatus camera_list_insert(CameraList *, Camera *);
Camera *camera_list_new_camera(CameraList *);

#endif
