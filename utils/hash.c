#include "hash.h"

// djb2 hash
uint32_t hash_djb2(const char *key) {

  unsigned long hash = 5381;
  int c;

  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}
