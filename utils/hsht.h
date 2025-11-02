#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include "backend/registry.h"

typedef enum {
  HashTableStatus_Success,
  HashTableStatus_AllocFail,
  HashTableStatus_UnfoundEntry,
  HashTableStatus_NotInit,
  HashTableStatus_UndefError,
} HashTableStatus;

typedef uint32_t hash_t;

typedef bool (*hsht_bucket_compare)(const void *, const void *);
typedef hash_t (*hsht_hash_generator)(const void *);
typedef bool (*hsht_bucket_get_occupied)(const void *);
typedef void (*hsht_bucket_set_occupied)(const void *, const bool);

typedef struct {
  const void *key;
  void *entry;
} HashTableOccupiedSlot;

typedef struct {

  const char *label;
  void *entries;
  size_t capacity;
  size_t type_size;

  hsht_bucket_compare comparator;
  hsht_hash_generator generator;
  hsht_bucket_get_occupied get_occupied;
  hsht_bucket_set_occupied set_occupied;

  struct {
    HashTableOccupiedSlot *entries;
    size_t capacity;
    size_t length;
  } occupied_list;

} HashTable;

typedef struct {
  const char *label;
  const size_t capacity;
  const size_t type_size;

  hsht_bucket_compare comparator_callback;
  hsht_hash_generator generator_callback;
  hsht_bucket_get_occupied get_occupied_callback;
  hsht_bucket_set_occupied set_occupied_callback;

} HashTableDescriptor;

static inline hash_t hsht_hash_ptr(const void *ptr) {
  uintptr_t key = (uintptr_t)ptr;

#if UINTPTR_MAX > 0xFFFFFFFF
  key = (uint32_t)(key ^ (key >> 32));
#endif

  uint32_t h = (uint32_t)key;

  h = (~h) + (h << 15);
  h = h ^ (h >> 12);
  h = h + (h << 2);
  h = h ^ (h >> 4);
  h = h * 2057;
  h = h ^ (h >> 16);

  return h;
}

static inline hash_t hsht_hash_id(uint32_t key) {
  key = ~key + (key << 15);
  key = key ^ (key >> 12);
  key = key + (key << 2);
  key = key ^ (key >> 4);
  key = key * 2057;
  key = key ^ (key >> 16);
  return key;
}

static inline hash_t hsht_hash_key(const char *key) {
  if (!key)
    return 0;
  uint32_t hash = 2166136261u;
  for (; *key; key++)
    hash = (hash ^ (uint8_t)(*key)) * 16777619u;
  return hash;
}

static inline hash_t hash_djb2(const char *key) {

  unsigned long hash = 5381;
  int c;

  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}

typedef enum {
  HashTableNewFlag_None = 0,
  HashTableNewFlag_FixedCapacity = 1,
} HashTableNewFlag;

HashTableStatus hsht_create(HashTable *, const HashTableDescriptor *);

void *hsht_new_entry(HashTable *, const void *, const HashTableNewFlag);

HashTableStatus hsht_remove_entry(HashTable *, const void *);

HashTableStatus hsht_expand(HashTable *, const size_t);
HashTableStatus hsht_rehash(HashTable *, const size_t);

void *hsht_find(HashTable *, const void *, size_t *);

HashTableStatus hsht_empty(void *, size_t *, size_t, const char *);

HashTableStatus hsht_destroy(void **, size_t *, size_t *, const char *);

#endif
