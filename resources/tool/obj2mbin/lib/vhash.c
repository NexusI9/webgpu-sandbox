#include "vhash.h"
#include "mbin.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static size_t vhash_hash(const vhash_value_t, size_t);

// Dj2B Polynomial rolling hash function
size_t vhash_hash(const vhash_value_t key, size_t capacity) {

  size_t hash = 2166136261ull; // FNV offset basis

  for (size_t i = 0; i < VERTEX_STRIDE; i++) {
    uint32_t bits;
    // bytes of key
    memcpy(&bits, &key[i], sizeof(bits));
    hash = hash * 16777619ull; // FNNV prime
    hash ^= bits;
  }

  return hash % capacity;
}

int vhash_insert(VertexHashTable *list, vhash_value_t key, mbin_index_t *idx) {

  // expand if length reach 75% capacity
  if (list->length >= list->capacity * 0.75) {

    size_t new_capacity = 2 * list->capacity;
    void *temp_entries =
        realloc(list->entries, new_capacity * sizeof(VertexHashKey));

    if (temp_entries) {
      list->entries = temp_entries;
      list->capacity = new_capacity;
    } else {
      perror("Couldn't expand list\n");
      return VHASH_ALLOC_FAILURE;
    }
  }

  if (vhash_search(list, key))
    // already exist
    return VHASH_EXIST;

  size_t index = vhash_hash(key, list->capacity);

  // if collision, then linear probing
  while (list->entries[index].occupied)
    index = (index + 1) % list->capacity;

  VertexHashKey *new_entry = &list->entries[index];
  // set vertex floats
  memcpy(new_entry->entries, key, sizeof(mbin_vertex_t) * VERTEX_STRIDE);

  // set bucket cached index
  new_entry->index = list->length;
  // update renferences index
  *idx = new_entry->index;

  // set bucked occupied state
  new_entry->occupied = true;

  list->length++;

  return VHASH_SUCCESS;
}

int vhash_create(VertexHashTable *list, size_t capacity) {
  list->entries = calloc(capacity, sizeof(VertexHashKey));

  if (list->entries == NULL) {
    perror("Couldn't create vertex hash table");
    return VHASH_ALLOC_FAILURE;
  }

  list->length = 0;
  list->capacity = capacity;
  return VHASH_SUCCESS;
}

VertexHashKey *vhash_search(VertexHashTable *list, vhash_value_t key) {

  size_t index = vhash_hash(key, list->capacity);
  size_t start = index;

  // check if values are equal
  while (list->entries[index].occupied) {

    if (memcmp(list->entries[index].entries, key,
               sizeof(mbin_vertex_t) * VERTEX_STRIDE) == 0)
      return &list->entries[index];

    index = (index + 1) % list->capacity;
    if (index == start)
      break;
  }

  return NULL;
}
