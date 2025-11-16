#ifndef _STATIC_LIST_UTILS_H_
#define _STATIC_LIST_UTILS_H_

#include <stddef.h>
#include <webgpu/webgpu.h>

typedef enum {
  StaticListStatus_Success,
  StaticListStatus_UnfoundEntry,
  StaticListStatus_MaxCapacity,
  StaticListStatus_OutOfRange,
  StaticListStatus_EmptyList,
} StaticListStatus;

StaticListStatus stli_create(size_t *, size_t *, const size_t, const char *);

StaticListStatus stli_insert(void *, size_t, size_t *, size_t, void *,
                             const char *);
StaticListStatus stli_remove(void *, size_t *, size_t, void *, const char *);

StaticListStatus stli_shift(void *, size_t *, size_t, const char *);

StaticListStatus stli_empty(void *, size_t *, size_t, const char *);

StaticListStatus stli_remove_at_index(void *, size_t *, size_t, size_t);

void *stli_new_entry(void *, size_t, size_t *, size_t, const char *);

void *stli_find(void *, size_t, size_t, void *, size_t *, const char *);

#endif
