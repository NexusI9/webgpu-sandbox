#include "vec3_list.h"

#include "utils/dyli.h"

DynamicListStatus vec3_list_create(Vec3List *list, size_t capacity) {
  return dyli_create((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(vec3), capacity, "Vector 3 list");
}

DynamicListStatus vec3_list_insert(Vec3List *list, vec3 entry) {
  return dyli_insert((void *)&list->entries, &list->capacity, &list->length,
                     sizeof(vec3), (void *)entry, 1, "Vector 3 list");
}

DynamicListStatus vec3_list_empty(Vec3List *list) {
  return dyli_empty((void *)list->entries, &list->length, sizeof(vec3));
}

void vec3_list_destroy(Vec3List *list) {}
