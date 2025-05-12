#include "edge.h"
#include "../utils/system.h"
#include "../utils/vector.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static size_t edge_key_hash(EdgeKey key, size_t capacity) {
  /*
    1. Use large number so even if a and b are similar they hit different
    numbers a ^ b or a + b give bad spread
    2. % capacity ensure the result fit in the bucket array (we actually use
    this hash as the index so it's necessary for it to be within the capacity
    range)
   */
  return ((size_t)key[0] * 7386093u ^ (size_t)key[1] * 19349663u) % capacity;
}

static bool edge_key_equal(EdgeKey a, EdgeKey b) {
  return a[0] == b[0] && a[1] == b[1];
}

void edge_hash_set_create(EdgeHashSet *set, size_t capacity) {

  set->entries = calloc(capacity, sizeof(EdgeBucket));
  set->occupied = calloc(capacity, sizeof(size_t));
  set->length = 0;

  if (set->entries == NULL) {
    VERBOSE_PRINT("Couldn't allocate memory for new hash set.\n");
    set->capacity = 0;
    return;
  }

  set->capacity = capacity;
}

void edge_hash_set_destroy(EdgeHashSet *set) {
  free(set->entries);
  free(set->occupied);
  set->occupied = NULL;
  set->entries = NULL;
  set->capacity = 0;
  set->length = 0;
}

bool edge_hash_set_insert(EdgeHashSet *set, EdgeKey key) {

  // check if entry doesn't already exists
  if (edge_hash_set_find(set, key))
    return false;

  // check capacity
  if (set->length >= set->capacity * 0.75) {
    /*
      If length reach 75% of capacity we expand list as beyond 75%
      the probing performance become heavier
     */
    size_t new_capacity = set->capacity * 2;
    EdgeBucket *temp = realloc(set->entries, new_capacity * sizeof(EdgeBucket));
    size_t *temp_occupied =
        realloc(set->occupied, new_capacity * sizeof(size_t));

    if (temp && temp_occupied) {
      set->entries = temp;
      set->occupied = temp_occupied;

      // set new entries to 0
      memset(&set->entries[set->capacity], 0,
             (new_capacity - set->capacity) * sizeof(EdgeBucket));

      memset(&set->occupied[set->capacity], 0,
             (new_capacity - set->capacity) * sizeof(size_t));

      set->capacity = new_capacity;
    } else {
      VERBOSE_PRINT(
          "Couldn't allocate memory for new hash set, new entry insertion "
          "aborted.\n");
      return false;
    }
  }

  // generate index from hash
  size_t index = edge_key_hash(key, set->capacity);
  while (set->entries[index].occupied)
    index = (index + 1) % set->capacity;

  // insert new bucket
  EdgeBucket *new_bucket = &set->entries[index];
  glm_ivec2_copy(key, new_bucket->key);
  new_bucket->occupied = true;

  // insert new occupied index
  set->occupied[set->length] = index;

  // update length
  set->length++;
  return true;
}

bool edge_hash_set_find(const EdgeHashSet *set, EdgeKey key) {

  // check if entry doesn't already exists
  size_t index = edge_key_hash(key, set->capacity);
  size_t start = index;

  while (set->entries[index].occupied) {

    // already exist
    if (edge_key_equal(key, set->entries[index].key))
      return true;

    index = (index + 1) % set->capacity;

    // full loop
    if (index == start)
      break;
  }

  return false;
}

bool edge_key_is_null(EdgeKey key) { return ivec2_equal(key, (ivec2){0, 0}); }
