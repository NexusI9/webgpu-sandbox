#ifndef _VINDEX_H_
#define _VINDEX_H_

#include "buffer.h"
#include "mbin.h"
#include "vattr.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define VINDEX_ATTRIBUTE_LINE_PREFIX "f "
#define VINDEX_SEPARATOR "/"
#define VINDEX_GROUP_SEPARATOR " "

#include <string.h>

typedef enum {
  VIndexStatus_Success,
  VIndexStatus_AllocFail,
} VIndexStatus;

#define VINDEX_DEFAULT_CAPACITY 64

/* Structure list
  List > Group > Attribute
 */

typedef mbin_index_t index_attribute[VERTEX_ATTRIBUTE_COUNT];

typedef struct {
  size_t capacity;
  size_t length;
  index_attribute *entries;
} IndexAttributeGroup;

typedef struct {
  size_t capacity;
  size_t length;
  IndexAttributeGroup *entries;
} IndexAttributeList;

typedef struct {
  IndexAttributeList *list;
  const char *pattern;
} VertexIndexCallbackDescriptor;

void index_attribute_cache(FILE *, IndexAttributeList *, const char *,
                           const char *);
VIndexStatus index_attribute_triangulate(IndexAttributeList *);
void index_attribute_position_list(IndexAttributeGroup *, mbin_index_t *,
                                   size_t *, size_t *);

void index_attribute_print(const IndexAttributeList *);

static inline void index_attribute_copy(index_attribute *src,
                                        index_attribute *dest) {
  memcpy(dest, src, sizeof(index_attribute));
}

void index_attribute_line_set_opposite(IndexAttributeList *);
void index_attribute_line_set_doublon(IndexAttributeList *);

#endif
