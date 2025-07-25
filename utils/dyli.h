#ifndef _DYNAMIC_LIST_H_
#define _DYNAMIC_LIST_H_

#include <stddef.h>
#include <stdio.h>

typedef enum {
  DynamicListStatus_Success,
  DynamicListStatus_AllocFail,
  DynamicListStatus_UnderError,
} DynamicListStatus;

typedef struct {
  void **entries;
  size_t *capacity;
  size_t *length;
  size_t type_size;
} DynamicList;

DynamicListStatus dyli_create(void **, size_t *, size_t *, size_t, size_t, const char *);
DynamicListStatus dyli_expand(void **, size_t *, size_t *, size_t, size_t, const char *);
void dyli_free(void **, size_t *, size_t *);

#endif
