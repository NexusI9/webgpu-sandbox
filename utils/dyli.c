#include "dyli.h"
#include "../utils/system.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/system.h"

/**
   Allocate necessary resource for dynamic list and update the capacity and
   length.
 */
DynamicListStatus dyli_create(void **entries, size_t *capacity, size_t *length,
                              size_t type_size, size_t num, const char *label) {

  *entries = calloc(num, type_size);
  *capacity = num;
  *length = 0;

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

  if (*entries == NULL || *capacity == 0) {
    VERBOSE_ERROR("Dynamic list '%s' not initialized, insertion aborted.",
                  label);
    return DynamicListStatus_NotInit;
  }

  if (*capacity < *length + count) {
    size_t new_capacity = (*capacity == 0) ? count : *capacity * 2;
    while (new_capacity < *length + count)
      if (dyli_expand(entries, capacity, length, type_size, 2, label) !=
          DynamicListStatus_Success)
        return DynamicListStatus_UndefError;
  }

  char *target = (char *)(*entries) + (*length * type_size);
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

DynamicListStatus dyli_remove_at_index(void *entries, size_t *length,
                                       size_t type_size, const size_t index,
                                       const char *label) {
  if (index >= *length)
    return DynamicListStatus_UnfoundEntry;

  void **list = (void **)entries;

  if (index < *length - 1)
    memmove(&list[index], &list[index + 1], (*length - index - 1) * type_size);

  (*length)--;
  return DynamicListStatus_Success;
}

void *dyli_new_entry(void **entries, size_t *capacity, size_t *length,
                     size_t type_size, const char *label) {

  // Ensure capacity first
  if (*length >= *capacity) {
    if (dyli_expand(entries, capacity, length, type_size, 2, label) !=
        DynamicListStatus_Success)
      return NULL;
  }

  // Compute pointer to new slot
  void *slot = (char *)(*entries) + ((*length) * type_size);

  // Zero-init for safety
  memset(slot, 0, type_size);

  // Increment length AFTER assignment
  (*length)++;

  return slot;
}

DynamicListStatus dyli_transfert(const void *src_entries,
                                 const size_t src_length, void **dest_entries,
                                 size_t *dest_capacity, size_t *dest_length,
                                 size_t type_size, const char *label) {

  while (*dest_length + src_length >= *dest_capacity) {

    size_t new_capacity =
        (*dest_capacity > 0) ? 2 * *dest_capacity : src_length;

    void *temp_entries = realloc(*dest_entries, new_capacity * type_size);

    if (temp_entries) {
      *dest_capacity = new_capacity;
      *dest_entries = temp_entries;

    } else {
      VERBOSE_ERROR("Couldn't transfert to %s.", label);
      return DynamicListStatus_AllocFail;
    }
  }

  memcpy((char *)(*dest_entries) + (*dest_length * type_size), src_entries,
         src_length * type_size);

  *dest_length += src_length;

  return DynamicListStatus_Success;
}
