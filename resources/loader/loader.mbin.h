#ifndef _LOADER_MBIN_H_
#define _LOADER_MBIN_H_

#include <stdint.h>

typedef uint32_t mbin_length_t;
typedef uint32_t mbin_size_t;
typedef uint32_t mbin_index_t;
typedef float mbin_vertex_t;

#pragma pack(push, 1)
typedef struct {
    mbin_length_t length;
    mbin_size_t size;
    mbin_vertex_t data[];
} MBINVertex;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct {
    mbin_length_t length;
    mbin_size_t size;
    mbin_index_t data[];
} MBINIndex;
#pragma pack(pop)



#endif
