#ifndef _DYNAMIC_LIST_H_
#define _DYNAMIC_LIST_H_

#include <stddef.h>
#include <stdio.h>

#define DYLI_INVALID_INDEX ((size_t)-1)

typedef enum {
  DynamicListStatus_Success,
  DynamicListStatus_AllocFail,
  DynamicListStatus_UnfoundEntry,
  DynamicListStatus_UndefError,
  DynamicListStatus_OutOfBound,
  DynamicListStatus_NotInit,
} DynamicListStatus;

typedef struct {
  void **entries;
  size_t *capacity;
  size_t *length;
  size_t type_size;
} DynamicList;

#ifdef __cplusplus
extern "C" {
#endif

DynamicListStatus dyli_create(void **, size_t *, size_t *, size_t, size_t,
                              const char *);
DynamicListStatus dyli_expand(void **, size_t *, size_t *, size_t, size_t,
                              const char *);
DynamicListStatus dyli_insert(void **, size_t *, size_t *, size_t, void *,
                              size_t, const char *);

DynamicListStatus dyli_append(const void *, const size_t, void **, size_t *,
                              size_t *, size_t, const char *);

DynamicListStatus dyli_replace(const void *, const size_t, void **, size_t *,
                               size_t *, size_t, const char *);

DynamicListStatus dyli_remove(void *, size_t *, size_t, void *, const char *);

DynamicListStatus dyli_remove_at_index(void *, size_t *, size_t, const size_t,
                                       const char *);

DynamicListStatus dyli_empty(void *, size_t *, size_t);

DynamicListStatus dyli_free(void **, size_t *, size_t *);

void *dyli_new_entry(void **, size_t *, size_t *, size_t, const char *);

#ifdef __cplusplus
}
#endif

#endif
