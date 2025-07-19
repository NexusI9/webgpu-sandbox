#include "dyli.h"
#include "../utils/system.h"
#include <stddef.h>
#include <stdlib.h>

/**
   Allocate necessary resource for dynamic list and update the capacity and
   length.
 */
int dyli_create(void **entries, size_t *capacity, size_t *length,
                size_t type_size, size_t size, const char *label) {

  *entries = calloc(*capacity, type_size);
  *length = 0;
  *capacity = size;

  if (*entries == NULL) {
    VERBOSE_ERROR("Couldn't create new dynamic list: %s\n", label);
    *capacity = 0;
    return VERTEX_GROUP_ALLOC_FAIL;
  }

  return DYNAMIC_LIST_SUCCESS;
}

/**
   Expand the dynamic list of 2n capacity.
 */
int dyli_expand(void **entries, size_t *capacity, size_t *length,
                size_t type_size, size_t scale, const char *label) {

  size_t new_capacity = scale * (*capacity);
  void *temp = (void *)realloc(*entries, new_capacity * type_size);

  if (temp == NULL) {
    VERBOSE_ERROR("Couldn't expand dynamic list: %s\n", label);
    return DYNAMIC_LIST_ALLOC_FAIL;
  }

  *entries = temp;
  *capacity = new_capacity;

  return DYNAMIC_LIST_SUCCESS;
}

void dyli_free(void **entries, size_t *capacity, size_t *length) {

  // free set
  free(*entries);
  *entries = NULL;
  *capacity = 0;
  *length = 0;
}
