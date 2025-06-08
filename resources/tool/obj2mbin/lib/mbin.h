#ifndef _MBIN_H_
#define _MBIN_H_

#define MBIN_ALLOC_FAIL 1
#define MBIN_SUCCESS 0

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint32_t mbin_int;
typedef float mbin_vertex_t;
typedef mbin_int mbin_index_t;
typedef mbin_int mbin_data_t;
typedef uint8_t vec_dimension_t;

typedef union {
  mbin_index_t u;
  mbin_vertex_t f;
} MBIN_U32Float;

//  Cache method "wireframe" automatically build triangles from the line
typedef enum {
  MBINIndexCacheMethod_Default,
  MBINIndexCacheMethod_Wireframe,
} MBINIndexCacheMethod;

/**
 Pramga pack 1 ensure no padding between the structure member.
 Tell compiler the structure members shouldn't be padded to fit memory
 alignmnent. Useful for memory mapped interface.

 However downside is that hardward is faster with aligned data.

 Default:

 Struct A{
   char AA;
   int BB;
   char CC;
 };


  |   1   |   2   |   3   |   4   |
  | AA(1) | pad.................. |
  | BB(1) | BB(2) | BB(3) | BB(4) |
  | CC(1) | pad.................. |

  Pack:

  |   1   |
  | AA(1) |
  | BB(1) |
  | BB(2) |
  | BB(3) |
  | BB(4) |
  | CC(1) |

 */

typedef struct {
  mbin_int vertex_length;
  mbin_int vertex_size_type;
  mbin_int index_length;
  mbin_int index_size_type;
} MBINFileCreateDescriptor;

#pragma pack(push, 1)
typedef struct {
  mbin_int vertex_length;
  mbin_int vertex_size_type;
  mbin_int index_length;
  mbin_int index_size_type;
  mbin_data_t data[]; // NOTE: Avoid using pointers (meaningless in for binary)
} MBINFile;
#pragma pack(pop)

int mbin_create(MBINFile **, const MBINFileCreateDescriptor *);
int mbin_write_buffer(const char *, MBINFile *);
void mbin_print(MBINFile *);
void mbin_free(MBINFile **);

#endif
