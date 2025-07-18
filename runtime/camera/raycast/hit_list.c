#include "hit_list.h"

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
    free(list->entries);
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

  
}
