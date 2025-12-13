#ifndef _FREE_LIST_H_
#define _FREE_LIST_H_

#include <stddef.h>

/**
 * Free list are fixed capacity lists that do not shift elements on deletion.
 */
typedef enum {
  FreeListStatus_Success,
  FreeListStatus_AllocFail,
  FreeListStatus_NotInit,
  FreeListStatus_OutOfBound,
  FreeListStatus_UnfoundEntry,
  FreeListStatus_UndefError
} FreeListStatus;

typedef struct {
  void *entries;
  size_t capacity;
  size_t count;
  size_t type_size;
  const char *label;
} FreeList;

FreeListStatus frli_create(void **, size_t *, size_t *, size_t, size_t,
                           const char *);

FreeListStatus frli_empty(void *, size_t *, size_t);

FreeListStatus frli_free(void **, size_t *, size_t *);

void *frli_new_entry(void **, size_t *, size_t *, size_t, size_t *,
                     const char *);

FreeListStatus frli_remove(void *, const size_t, size_t *, size_t, void *,
                           const char *);

FreeListStatus frli_remove_at_index(void *, const size_t, size_t *, size_t,
                                    const size_t, const char *);

FreeListStatus frli_append(const void *, const size_t, void **, size_t *,
                           size_t *, size_t, const char *);

FreeListStatus frli_replace(const void *, const size_t, void **, size_t *,
                            size_t *, size_t, const char *);

FreeListStatus frli_clone(const void *, const size_t, void **, size_t *,
                          size_t *, size_t, const char *);
#endif
