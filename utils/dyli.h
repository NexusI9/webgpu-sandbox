#ifndef _DYNAMIC_LIST_H_
#define _DYNAMIC_LIST_H_

#include <stddef.h>
#include <stdio.h>

#define DYNAMIC_LIST_SUCCESS 0
#define DYNAMIC_LIST_ALLOC_FAIL 1
#define DYNAMIC_LIST_UNDEF_ERROR 2

typedef struct {
  void **entries;
  size_t *capacity;
  size_t *length;
  size_t type_size;
} DynamicList;

int dyli_create(void **, size_t *, size_t *, size_t, size_t, const char *);
int dyli_expand(void **, size_t *, size_t *, size_t, size_t, const char *);
void dyli_free(void **, size_t *, size_t *);

#endif
