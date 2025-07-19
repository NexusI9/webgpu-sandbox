#include "dyli.h"
#include "../utils/system.h"
#include <stddef.h>
#include <stdlib.h>

/**
   Allocate necessary resource for dynamic list and update the capacity and
   length.
 */
int dyli_create(const DynamicList *list, size_t capacity, const char *label) {

  *list->entries = calloc(*list->capacity, sizeof(VertexGroup));
  *list->length = 0;
  *list->capacity = capacity;

  if (*list->entries == NULL) {
    VERBOSE_ERROR("Couldn't create new dynamic list: %s\n", label);
    *list->capacity = 0;
    return VERTEX_GROUP_ALLOC_FAIL;
  }

  return DYNAMIC_LIST_SUCCESS;
}

/**
   Expand the dynamic list of 2n capacity.
 */
int dyli_expand(const DynamicList *list, size_t scale, const char *label) {

  size_t new_capacity = scale * (*list->capacity);
  void *temp = (void *)realloc(*list->entries, new_capacity * list->type_size);

  if (temp == NULL) {
    VERBOSE_ERROR("Couldn't expand dynamic list: %s\n", label);
    return DYNAMIC_LIST_ALLOC_FAIL;
  }

  *list->entries = temp;
  *list->capacity = new_capacity;

  return DYNAMIC_LIST_SUCCESS;
}

void dyli_free(const DynamicList *list) {

  // free set
  free(*list->entries);
  *list->entries = NULL;
  *list->capacity = 0;
  *list->length = 0;
}
