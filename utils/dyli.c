#include "dyli.h"
#include "../utils/system.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
   Allocate necessary resource for dynamic list and update the capacity and
   length.
 */
DynamicListStatus dyli_create(void **entries, size_t *capacity, size_t *length,
                              size_t type_size, size_t num, const char *label) {

  *entries = calloc(num, type_size);
  *length = 0;
  *capacity = num;

  if (*entries == NULL) {
    VERBOSE_ERROR("Couldn't create new dynamic list: %s\n", label);
    *capacity = 0;
    return DynamicListStatus_AllocFail;
  }

  return DynamicListStatus_Success;
}

/**
   Expand the dynamic list of 2n capacity.
 */
DynamicListStatus dyli_expand(void **entries, size_t *capacity, size_t *length,
                              size_t type_size, size_t scale,
                              const char *label) {

  size_t new_capacity = scale * (*capacity);
  void *temp = (void *)realloc(*entries, new_capacity * type_size);

  if (temp == NULL)
    return DynamicListStatus_AllocFail;

  *entries = temp;
  *capacity = new_capacity;

  return DynamicListStatus_Success;
}

DynamicListStatus dyli_insert(void **entries, size_t *capacity, size_t *length,
                              size_t type_size, void *entry, size_t count,
                              const char *label) {

  if (*capacity < *length + count) {

    size_t new_capacity = (*capacity == 0) ? count : *capacity * 2;
    while (new_capacity < *length + count)
      new_capacity *= 2;

    if (dyli_expand(entries, capacity, length, type_size, new_capacity,
                    label) != DynamicListStatus_Success)
      return DynamicListStatus_UndefError;
  }

  void *target = (char *)(*entries) + (*length * type_size);
  memcpy(target, entry, type_size * count);
  *length += count;

  return DynamicListStatus_Success;
}

DynamicListStatus dyli_empty(void *entries, size_t *length, size_t type_size) {

  memset(entries, 0, (*length) * type_size);
  *length = 0;
  return DynamicListStatus_Success;
}

DynamicListStatus dyli_free(void **entries, size_t *capacity, size_t *length) {

  // free set
  free(*entries);
  *entries = NULL;
  *capacity = 0;
  *length = 0;

  return DynamicListStatus_Success;
}

DynamicListStatus dyli_remove(void *entries, size_t *length, size_t type_size,
                              void *entry, const char *label) {

  void **list = (void **)entries;
  void *target = *(void **)entry;

  for (size_t i = 0; i < *length; i++) {

    if (list[i] == target) {

      if (i < *length - 1)
        memmove(&list[i], &list[i + 1], (*length - i - 1) * type_size);

      (*length)--;
      return DynamicListStatus_Success;
    }
  }

  return DynamicListStatus_UnfoundEntry;
}
