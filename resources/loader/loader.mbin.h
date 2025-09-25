#ifndef _LOADER_MBIN_H_
#define _LOADER_MBIN_H_

#include <stdint.h>

#include "runtime/geometry/vertex/vertex.h"
#include "runtime/primitive/primitive.h"
#include "resources/tool/obj2mbin/lib/mbin.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/primitive/core.h"

typedef uint32_t mbin_length_t;

/* TODO: unify mesh loading system:
   1. either directly push mesh in scene list
   2. or provide data to create scene mesh (++ flexibility)
 */

typedef enum {
  MBINLoaderStatus_Success,
  MBINLoaderStatus_AllocFail,
  MBINLoaderStatus_UndefError,
} MBINLoaderStatus;

typedef struct {
  const char *path;
  VertexAttribute *vertex;
  VertexIndex *index;
} MBINLoadDescriptor;

typedef struct {
  const char *path;
  Primitive *primitive;
} MBINLoadPrimitiveDescriptor;

MBINLoaderStatus loader_mbin_load(MBINFile **, const char *);
MBINLoaderStatus loader_mbin_load_primitive(MBINLoadPrimitiveDescriptor *);

#endif
