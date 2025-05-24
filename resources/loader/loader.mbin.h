#ifndef _LOADER_MBIN_H_
#define _LOADER_MBIN_H_

#include "../geometry/vertex.h"
#include "../primitive/primitive.h"
#include "../tool/obj2mbin/lib/mbin.h"
#include <stdint.h>

typedef uint32_t mbin_length_t;
typedef uint32_t mbin_size_t;
typedef vindex_t mbin_index_t;
typedef vattr_t mbin_vertex_t;

/* TODO: unify mesh loading system:
   1. either directly push mesh in scene list
   2. or provide data to create scene mesh (++ flexibility)
 */

#define MBIN_LOADER_SUCCESS 0
#define MBIN_LOADER_ALLOC_FAIL 1
#define MBIN_LOADER_LOAD_ERROR 2

typedef struct {
  const char *path;
  VertexAttribute *vertex;
  VertexIndex *index;
} MBINLoadDescriptor;

typedef struct {
  const char *path;
  Primitive *primitive;
} MBINLoadPrimitiveDescriptor;

int loader_mbin_load(MBINLoadDescriptor *);
int loader_mbin_load_primitive(MBINLoadPrimitiveDescriptor *);

#endif
