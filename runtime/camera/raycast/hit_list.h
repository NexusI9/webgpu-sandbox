#ifndef _CAMERA_RAYCAST_HIT_LIST_H_
#define _CAMERA_RAYCAST_HIT_LIST_H_

#include "../../mesh/mesh.h"

#define CAMERA_RAYCAST_HIT_LIST_SUCCESS 0
#define CAMERA_RAYCAST_HIT_LIST_ALLOC_FAIL 1
#define CAMERA_RAYCAST_HIT_LIST_UNDEF_ERROR 2
#define CAMERA_RAYCAST_HIT_LIST_MAX_HIT 128

typedef struct {
  Mesh *mesh;
  float distance;
} CameraRaycastHit;

typedef struct {
  CameraRaycastHit* entries;
  size_t capacity;
  size_t length;
} CameraRaycastHitList;


int camera_raycast_hit_list_create(CameraRaycastHitList*, size_t);
void camera_raycast_hit_list_empty(CameraRaycastHitList*);
void camera_raycast_hit_list_sort(CameraRaycastHitList*);

#endif
