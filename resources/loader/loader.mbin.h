#ifndef _LOADER_MBIN_H_
#define _LOADER_MBIN_H_

#include "../geometry/vertex.h"
#include <stdint.h>

typedef uint32_t mbin_length_t;
typedef uint32_t mbin_size_t;
typedef vindex_t mbin_index_t;
typedef vattr_t mbin_vertex_t;

#pragma pack(push, 1)
typedef struct {
  mbin_size_t type_size;
  mbin_length_t length;
  mbin_vertex_t data[];
} MBINVertex;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
  mbin_size_t type_size;
  mbin_length_t length;
  mbin_index_t data[];
} MBINIndex;
#pragma pack(pop)

/* TODO: unify mesh loading system:
   1. either directly push mesh in scene list
   2. or provide data to create scene mesh (++ flexibility)
 */

typedef struct {
  const char *vertex_path;
  const char *index_path;
  MBINVertex *vertex_data;
  MBINIndex *index_data;

} MBINLoadDescriptor;

void loader_mbin_load(MBINLoadDescriptor *);

#endif
