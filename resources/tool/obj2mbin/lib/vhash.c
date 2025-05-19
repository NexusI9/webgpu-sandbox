#include "vhash.h"
#include "mbin.h"
#include <stdint.h>
#include <string.h>

static size_t vhash_hash(const vhash_value_t, size_t);

// Dj2B Polynomial rolling hash function
size_t vhash_hash(const vhash_value_t key, size_t capacity) {

  size_t hash = 2166136261u; // FNV offset basis

  for (size_t i = 0; i < VERTEX_STRIDE; i++) {
    uint32_t bits;
    // bytes of key
    memcpy(&bits, &key[i], sizeof(bits));
    hash = hash * 16777619u; // FNNV prime
    hash ^= bits;
  }

  return hash % capacity;
}

int vhash_insert(VertexHashTable *list, vhash_value_t key) {

  if (vhash_search(list, key))
    // already exist
    return 2;

  // expand if length reach 75% capacity
  if (list->length == list->capacity * 0.75) {

    size_t new_capacity = 2 * list->capacity;
    void *temp = realloc(list->entries, new_capacity);

    if (temp) {
      list->entries = temp;
      list->capacity = new_capacity;
    } else {
      perror("Couldn't expand list\n");
      return 1;
    }
  }

  size_t index = vhash_hash(key, list->capacity);

  // if collision, then linear probing
  while (list->entries[index].occupied)
    index = (index + 1) % list->capacity;

  VertexHashKey *new_entry = &list->entries[index];
  memcpy(new_entry->entries, key, sizeof(mbin_vertex_t) * VERTEX_STRIDE);
  new_entry->occupied = true;

  list->length++;

  return 0;
}

int vhash_create(VertexHashTable *list, size_t capacity) {
  list->entries = calloc(capacity, sizeof(vhash_value_t));

  if (list->entries == NULL) {
    perror("Couldn't create vertex hash table");
    return 1;
  }

  list->length = 0;
  list->capacity = capacity;
  return 0;
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
