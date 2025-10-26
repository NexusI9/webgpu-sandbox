#ifndef _UTILS_MEM_H_
#define _UTILS_MEM_H_

#define STRUCT_PAD(alignment, size_bytes) \
    (((alignment) - ((size_bytes) % (alignment))) % (alignment) / sizeof(float))


#endif
