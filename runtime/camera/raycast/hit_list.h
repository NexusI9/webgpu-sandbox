#ifndef _CAMERA_RAYCAST_HIT_LIST_H_
#define _CAMERA_RAYCAST_HIT_LIST_H_

#include <stddef.h>

#include "../../mesh/mesh.h"
#include "../runtime/mesh/core.h"
#include "../runtime/mesh/core.h"

typedef enum {
  CameraRaycastHitListStatus_Success,
  CameraRaycastHitListStatus_AllocFail,
  CameraRaycastHitListStatus_UndefError,
} CameraRaycastHitListStatus;

#define CAMERA_RAYCAST_HIT_LIST_MAX_HIT 128

typedef struct {
  Mesh *mesh;
  float distance;
} CameraRaycastHit;

typedef struct {
  CameraRaycastHit *entries;
  size_t capacity;
  size_t length;
} CameraRaycastHitList;

CameraRaycastHitListStatus camera_raycast_hit_list_create(CameraRaycastHitList *, size_t);
void camera_raycast_hit_list_empty(CameraRaycastHitList *);
void camera_raycast_hit_list_sort(CameraRaycastHitList *);

#endif
