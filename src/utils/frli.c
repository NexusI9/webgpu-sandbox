#include "frli.h"
#include "backend/logger.h"
#include <stddef.h>
#include <stdlib.h>

/**
 * Allocate the base memory for a free list.
 */
FreeListStatus frli_create(void **entries, size_t *capacity, size_t *count,
                           size_t type_size, size_t num, const char *label) {

  *entries = calloc(num, type_size);
  *capacity = num;
  *count = 0;

  if (*entries == NULL) {
    logger_add(LoggerFlag_Error, "Couldn't create new free list: %s\n", label);
    *capacity = 0;
    return FreeListStatus_AllocFail;
  }

  return FreeListStatus_Success;
}

/**
 * Empty all entries without freeing memory.
 */
FreeListStatus frli_empty(void *entries, size_t *count, size_t type_size) {
  memset(entries, 0, (*count) * type_size);
  *count = 0;
  return FreeListStatus_Success;
}

/**
 * Free the entire list.
 */
FreeListStatus frli_free(void **entries, size_t *capacity, size_t *count) {
  free(*entries);
  *entries = NULL;
  *capacity = 0;
  *count = 0;
  return FreeListStatus_Success;
}

/**
 * Get a new slot from the free list.
 * Reuses any empty slot (zeroed), otherwise expands.
 */
void *frli_new_entry(void **entries, size_t *capacity, size_t *count,
                     size_t type_size, size_t *index, const char *label) {

  if (*entries == NULL || *capacity == 0) {
    logger_add(LoggerFlag_Error,
               "Free list '%s' not initialized, new entry aborted. "
               "(Entries: <%p>, capacity: %lu, count: %lu)",
               label, *entries, *capacity, *count);
    return NULL;
  }

  // Expand if full
  if (*count == *capacity) {
    logger_add(LoggerFlag_Error, "Free list '%s' reached max capacity");
    return NULL;
  }

  // Try to find a free slot (a fully zeroed entry)
  for (size_t i = 0; i < *capacity; i++) {
    char *slot = (char *)(*entries) + (i * type_size);
    bool is_free = true;

    for (size_t b = 0; b < type_size; b++) {
      if (slot[b] != 0) {
        is_free = false;
        break;
      }
    }

    if (is_free) {
      memset(slot, 0, type_size);

      if (index)
        *index = i;

      (*count)++;
      return slot;
    }
  }

  return NULL;
}

/**
 * Remove an entry by pointer, marking it as free (zeroed).
 * Does NOT shift memory.
 */
FreeListStatus frli_remove(void *entries, const size_t capacity, size_t *count,
                           size_t type_size, void *entry, const char *label) {

  for (size_t i = 0; i < capacity; i++) {
    char *slot = (char *)entries + (i * type_size);
    if (slot == entry) {
      memset(slot, 0, type_size);
      (*count)--;
      return FreeListStatus_Success;
    }
  }

  logger_add(LoggerFlag_Warning, "Entry not found in '%s'.", label);
  return FreeListStatus_UnfoundEntry;
}

/**
 * Remove by index (mark as free instead of shifting).
 */
FreeListStatus frli_remove_at_index(void *entries, const size_t capacity,
                                    size_t *count, size_t type_size,
                                    const size_t index, const char *label) {
  if (index >= capacity)
    return FreeListStatus_OutOfBound;

  char *slot = (char *)entries + (index * type_size);
  memset(slot, 0, type_size);

  (*count)--;
  return FreeListStatus_Success;
}

/**
 * Append entries (used for cloning or merging free lists).
 */
FreeListStatus frli_append(const void *src_entries, const size_t src_count,
                           void **dest_entries, size_t *dest_capacity,
                           size_t *dest_count, size_t type_size,
                           const char *label) {

  if (*dest_count + src_count >= *dest_capacity) {
    logger_add(LoggerFlag_Error,
               "The source list has a count reach out of bound destination's "
               "capacity list. (%lu against %lu).",
               src_count, *dest_count);
    return FreeListStatus_OutOfBound;
  }

  memcpy((char *)(*dest_entries) + (*dest_count * type_size), src_entries,
         src_count * type_size);
  *dest_count += src_count;
  return FreeListStatus_Success;
}

/**
 * Replace all entries in destination.
 */
FreeListStatus frli_replace(const void *src_entries, const size_t src_count,
                            void **dest_entries, size_t *dest_capacity,
                            size_t *dest_count, size_t type_size,
                            const char *label) {

  if (src_count > *dest_capacity) {
    logger_add(
        LoggerFlag_Error,
        "The source list has a count greater that the destination's capacity "
        "list. (%lu against %lu).",
        src_count, *dest_count);
    return FreeListStatus_OutOfBound;
  }

  memcpy(*dest_entries, src_entries, src_count * type_size);
  *dest_count = src_count;
  return FreeListStatus_Success;
}

/**
 * Clone list contents.
 */
FreeListStatus frli_clone(const void *src_entries, const size_t src_count,
                          void **dest_entries, size_t *dest_capacity,
                          size_t *dest_count, size_t type_size,
                          const char *label) {

  size_t new_capacity = src_count;
  void *temp = malloc(new_capacity * type_size);
  if (!temp) {
    logger_add(LoggerFlag_Error, "Couldn't clone to %s.", label);
    return FreeListStatus_AllocFail;
  }

  memcpy(temp, src_entries, src_count * type_size);
  free(*dest_entries);
  *dest_entries = temp;
  *dest_capacity = new_capacity;
  *dest_count = src_count;

  return FreeListStatus_Success;
}
