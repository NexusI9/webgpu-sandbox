#include "hit_list.h"
#include "string.h"
#include <stdlib.h>

static int camera_raycast_hit_list_sort_func(const void *a, const void *b) {

  const CameraRaycastHit *hit_a = (const CameraRaycastHit *)a;
  const CameraRaycastHit *hit_b = (const CameraRaycastHit *)b;

  if (hit_a->distance < hit_b->distance)
    return -1;
  if (hit_a->distance > hit_b->distance)
    return 1;

  return 0;
}

/**
   Allocate hit list entries and define base parameters
 */
int camera_raycast_hit_list_create(CameraRaycastHitList *list,
                                   size_t capacity) {

  list->entries = calloc(capacity, sizeof(CameraRaycastHit));
  list->capacity = capacity;
  list->length = 0;

  if (list->entries == NULL) {
    perror("Couldn't allocate camera raycast hit list entries.\n");
    list->capacity = 0;
    list->length = 0;
    return CAMERA_RAYCAST_HIT_LIST_ALLOC_FAIL;
  }

  return CAMERA_RAYCAST_HIT_LIST_SUCCESS;
}

/**
   Empty the hit list, used during the check bound callback.
   We need to empty the hit list each check to refresh the hit mesh and their
   sorting.
 */
void camera_raycast_hit_list_empty(CameraRaycastHitList *list) {

  if (list->entries != NULL) {
    memset(list->entries, 0, list->capacity * sizeof(CameraRaycastHit));
    list->length = 0;
  }
}

/**
   Hit list meshes are sorted from the closest to the furthest.
   We need to sort mesh by distance since initially mesh reference list are
   provided in a certain order, however this order doesn't reflect the actual
   distance from the ray. I.E. an object in the last reference list can actually
   be the closes one to the camera.
   Thus we need to compare all hit meshes distance from ray origin.
   Note that the distance is NOT based on mesh mere position but on ray hit with
   mesh surface.
 */
void camera_raycast_hit_list_sort(CameraRaycastHitList *list) {

  // none or only 1 entry, skip comparison
  if (list->length < 2 || list->entries == NULL)
    return;

  qsort(list->entries, list->length, sizeof(CameraRaycastHit),
        camera_raycast_hit_list_sort_func);
}
