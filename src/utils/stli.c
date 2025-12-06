#include "stli.h"
#include <string.h>

StaticListStatus stli_create(size_t *capacity, size_t *length, const size_t num,
                             const char *label) {

  *capacity = num;
  *length = 0;

  return StaticListStatus_Success;
}

StaticListStatus stli_insert(void *entries, size_t capacity, size_t *length,
                             size_t type_size, const void *entry,
                             const char *label) {

  if (capacity == *length)
    return StaticListStatus_MaxCapacity;

  memcpy((char *)entries + (*length)++ * type_size, (char *)entry, type_size);

  return StaticListStatus_Success;
}

StaticListStatus stli_remove(void *entries, size_t *length, size_t type_size,
                             const void *entry, const char *label) {

  if (*length == 0)
    return StaticListStatus_EmptyList;

  size_t index = 0;
  if (stli_find(entries, *length, type_size, entry, &index, label) != NULL) {

    if (index < *length - 1) {
      memmove((char *)entries + index * type_size,
              (char *)entries + (index + 1) * type_size,
              (*length - 1 - index) * type_size);
    }

    (*length)--;
    return StaticListStatus_Success;
  }

  return StaticListStatus_UnfoundEntry;
}

StaticListStatus stli_shift(void *entries, size_t *length, size_t type_size,
                            const char *label) {

  if (*length == 0)
    return StaticListStatus_EmptyList;

  memmove((char *)entries, (char *)entries + type_size,
          (*length - 1) * type_size);

  (*length)--;

  return StaticListStatus_Success;
}

StaticListStatus stli_empty(void *entries, size_t *length, size_t type_size,
                            const char *label) {

  memset((char *)entries, 0, *length * type_size);
  *length = 0;

  return StaticListStatus_Success;
}

void *stli_new_entry(void *entries, size_t capacity, size_t *length,
                     size_t type_size, const char *label) {

  if (capacity == *length)
    return NULL;

  return (char *)entries + type_size * (*length)++;
}

void *stli_find(void *entries, size_t length, size_t type_size,
                const void *entry, size_t *index, const char *label) {

  for (size_t i = 0; i < length; i++)
    if (memcmp((char *)entries + i * type_size, entry, type_size) == 0) {
      if (index)
        *index = i;
      return (char *)entries + i * type_size;
    }

  return NULL;
}

StaticListStatus stli_remove_at_index(void *entries, size_t *length,
                                      size_t type_size, size_t index,
                                      const char *) {
  if (*length == 0) {
    return StaticListStatus_EmptyList;
  }
  if (index >= *length) {
    return StaticListStatus_OutOfRange;
  }

  if (index < *length - 1) {
    memmove((char *)entries + index * type_size,
            (char *)entries + (index + 1) * type_size,
            (*length - 1 - index) * type_size);
  }

  (*length)--;
  return StaticListStatus_Success;
}
