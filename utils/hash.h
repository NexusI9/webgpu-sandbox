#ifndef _HASH_H_
#define _HASH_H_

#include <stdint.h>

typedef uint32_t hash_djb2_t;

uint32_t hash_djb2(const char*);

#endif
