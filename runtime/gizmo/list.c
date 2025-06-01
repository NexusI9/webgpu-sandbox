#include "list.h"
#include "camera.h"
#include "light_ambient.h"
#include "light_point.h"
#include "light_spot.h"
#include "light_sun.h"
#include "string.h"

static void *gizmo_list_expand(void **, size_t, size_t, size_t *);
static void *gizmo_list_alloc(void **, size_t, size_t, size_t *, size_t *);
static int gizmo_list_insert(const GizmoListInsertDescriptor *);
static void *gizmo_list_new(const GizmoListNewDescriptor *);

void *gizmo_list_expand(void **dest, size_t capacity, size_t type_size,
                        size_t *new_capacity) {

  void *temp = realloc(dest, capacity * type_size);

  if (temp == NULL) {
    perror("Couldn't expand gizmo list\n");
    return temp;
  }

  *dest = temp;
  *new_capacity = capacity;

  return dest;
}

void *gizmo_list_alloc(void **dest, size_t capacity, size_t type_size,
                       size_t *new_capacity, size_t *new_length) {

  *dest = malloc(capacity * type_size);

  if (*dest == NULL) {
    perror("Couldn't allocate memory for gizmo list\n");
    *new_capacity = 0;
    *new_length = 0;
    return *dest;
  }

  *new_capacity = capacity;
  *new_length = 0;

  return *dest;
}

/**
   Dynamically allocate and initialize the gizmo lists (point, ambient,
   camera...)
 */
int gizmo_list_create(GizmoList *list, size_t capacity) {

  // create point light
  gizmo_list_alloc((void **)&list->point_light.entries, capacity,
                   sizeof(GizmoPointLight), &list->point_light.capacity,
                   &list->point_light.length);

  // create sun light
  gizmo_list_alloc((void **)&list->sun_light.entries, capacity,
                   sizeof(GizmoSunLight), &list->sun_light.capacity,
                   &list->sun_light.length);

  // create ambient light
  gizmo_list_alloc((void **)&list->ambient_light.entries, capacity,
                   sizeof(GizmoAmbientLight), &list->ambient_light.capacity,
                   &list->ambient_light.length);

  // create spot light
  gizmo_list_alloc((void **)&list->spot_light.entries, capacity,
                   sizeof(GizmoSpotLight), &list->spot_light.capacity,
                   &list->spot_light.length);

  // create camera
  gizmo_list_alloc((void **)&list->camera.entries, capacity,
                   sizeof(GizmoCamera), &list->camera.capacity,
                   &list->camera.length);

  return GIZMO_LIST_SUCCESS;
}

/**
   Function that abstracts the gizmo list insertion process.
   Inserts a given item in the list entries depending on its type size.
 */
int gizmo_list_insert(const GizmoListInsertDescriptor *desc) {

  // check if list is init
  if (desc->entries == NULL) {
    perror("Gizmo list not initialized\n");
    return GIZMO_LIST_ERROR;
  }

  size_t new_capacity = 2 * (*desc->capacity);
  // check list capacity
  if (*desc->length == *desc->capacity &&
      gizmo_list_expand(desc->entries, new_capacity, sizeof(desc->type_size),
                        desc->capacity) != GIZMO_LIST_SUCCESS)
    return GIZMO_LIST_ERROR;

  // copy item to list memory
  memcpy(&desc->entries[(*desc->length)++], desc->new_entry,
         sizeof(desc->type_size));

  return GIZMO_LIST_SUCCESS;
}

/**
   Function that abstracts the gizmo list get new item process.
   Return the pointer to the newly "created" element
 */
void *gizmo_list_new(const GizmoListNewDescriptor *desc) {

  // check if list is init
  if (desc->entries == NULL) {
    perror("Gizmo list not initialized\n");
    return NULL;
  }

  size_t new_capacity = 2 * (*desc->capacity);
  // check list capacity
  if (*desc->length == *desc->capacity &&
      gizmo_list_expand(desc->entries, new_capacity, sizeof(desc->type_size),
                        desc->capacity) != GIZMO_LIST_SUCCESS)
    return NULL;

  return desc->entries[(*desc->length)++];
}

/**
   Insert given light to the point light gizmo
 */
int gizmo_list_insert_point_light(GizmoList *list, GizmoPointLight *gizmo) {

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoPointLight),
      .new_entry = gizmo,
      .capacity = &list->point_light.capacity,
      .length = &list->point_light.length,
      .entries = (void **)&list->point_light.entries,
  });
}
GizmoPointLight *gizmo_list_new_point_light(GizmoList *list) {
  return (GizmoPointLight *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoPointLight),
      .capacity = &list->point_light.capacity,
      .length = &list->point_light.length,
      .entries = (void **)&list->point_light.entries,
  });
}

// ambient light gizmo
int gizmo_list_insert_ambient_light(GizmoList *list, GizmoAmbientLight *gizmo) {

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoAmbientLight),
      .new_entry = gizmo,
      .capacity = &list->ambient_light.capacity,
      .length = &list->ambient_light.length,
      .entries = (void **)&list->ambient_light.entries,
  });
}
GizmoAmbientLight *gizmo_list_new_ambient_light(GizmoList *list) {
  return (GizmoAmbientLight *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoAmbientLight),
      .capacity = &list->ambient_light.capacity,
      .length = &list->ambient_light.length,
      .entries = (void **)&list->ambient_light.entries,
  });
}

// sun light gizmo
int gizmo_list_insert_sun_light(GizmoList *list, GizmoSunLight *gizmo) {

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoSunLight),
      .new_entry = gizmo,
      .capacity = &list->sun_light.capacity,
      .length = &list->sun_light.length,
      .entries = (void **)&list->sun_light.entries,
  });
}
GizmoSunLight *gizmo_list_new_sun_light(GizmoList *list) {
  return (GizmoSunLight *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoSunLight),
      .capacity = &list->sun_light.capacity,
      .length = &list->sun_light.length,
      .entries = (void **)&list->sun_light.entries,
  });
}

// spot light gizmo
int gizmo_list_insert_spot_light(GizmoList *list, GizmoSpotLight *gizmo) {

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoSpotLight),
      .new_entry = gizmo,
      .capacity = &list->spot_light.capacity,
      .length = &list->spot_light.length,
      .entries = (void **)&list->spot_light.entries,
  });
}

GizmoSpotLight *gizmo_list_new_spot_light(GizmoList *list) {
  return (GizmoSpotLight *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoSpotLight),
      .capacity = &list->spot_light.capacity,
      .length = &list->spot_light.length,
      .entries = (void **)&list->sun_light.entries,
  });
}

// camera gizmo
int gizmo_list_insert_camera(GizmoList *list, GizmoCamera *gizmo) {

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoCamera),
      .new_entry = gizmo,
      .capacity = &list->camera.capacity,
      .length = &list->camera.length,
      .entries = (void **)&list->camera.entries,
  });
}

GizmoCamera *gizmo_list_new_camera(GizmoList *list) {
  return (GizmoCamera *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoCamera),
      .capacity = &list->camera.capacity,
      .length = &list->camera.length,
      .entries = (void **)&list->camera.entries,
  });
}
