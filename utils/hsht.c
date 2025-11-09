#include "hsht.h"
#include "utils/dyli.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static inline DynamicListStatus
hsht_register_occupied_entry(HashTable *, const void *, void *);

static inline DynamicListStatus hsht_unregister_occupied_entry(HashTable *,
                                                               const void *);

HashTableStatus hsht_create(HashTable *table, const HashTableDescriptor *desc) {

  table->entries = calloc(desc->capacity, desc->type_size);
  table->capacity = desc->capacity;

  if (table->entries == NULL) {
    logger_add(LoggerFlag_Error, "Couldn't create new hash table: %s\n",
               desc->label);
    table->capacity = 0;
    return HashTableStatus_AllocFail;
  }

  table->type_size = desc->type_size;
  table->label = desc->label;

  table->comparator = desc->comparator_callback;
  table->generator = desc->generator_callback;
  table->get_occupied = desc->get_occupied_callback;
  table->set_occupied = desc->set_occupied_callback;

  // keep track of occupied slot in a linear dynamic list so it's easier to
  // rehash on expand.
  if (dyli_create((void *)&table->occupied_list.entries,
                  &table->occupied_list.capacity, &table->occupied_list.length,
                  sizeof(HashTableOccupiedSlot), desc->capacity,
                  desc->label) != DynamicListStatus_Success) {
    logger_add(LoggerFlag_Error,
               "Couldn't create new hash table '%s' occupied list.\n",
               desc->label);
    return HashTableStatus_AllocFail;
  }

  return HashTableStatus_Success;
}

/**
   Overall flow:
   - Traverse occupied slots
   - Get key and generate hash % old_capacity
   - Temporarily Cache the data from the slot
   - Reset slot back to 0
   - Generate hash % current_capacity
   - copy cached data to new slot
 */
HashTableStatus hsht_rehash(HashTable *table, const size_t previous_capacity) {

  for (size_t i = 0; i < table->occupied_list.length; i++) {

    HashTableOccupiedSlot *slot = &table->occupied_list.entries[i];

    hash_t old_hash = table->generator(slot->key) % previous_capacity;
    void *cached_slot = malloc(table->type_size);

    if (cached_slot == NULL) {
      logger_add(LoggerFlag_Error,
                 "Unable to allocate resources to Hash Table '%s' rehashing.\n",
                 table->label);
      return HashTableStatus_AllocFail;
    }

    memcpy(cached_slot, slot->entry, table->type_size); // cache
    memset((void *)slot->entry, 0, table->type_size);   // clean

    void *new_entry = hsht_new_entry(table, slot->key, HashTableNewFlag_None);

    if (new_entry == NULL) {
      logger_add(
          LoggerFlag_Error,
          "Unable to create new entry while rehashing hash table '%s'.\n",
          table->label);
      return HashTableStatus_UndefError;
    }

    memcpy(new_entry, cached_slot, table->type_size);

    free(cached_slot);
  }

  return HashTableStatus_Success;
}

HashTableStatus hsht_expand(HashTable *table, const size_t scale) {

  if (table->capacity == 0) {
    logger_add(LoggerFlag_Error,
               "Hash table '%s' has a capaicty of 0. Make sure it's been "
               "initialized corectly.\n",
               table->label);
    return HashTableStatus_NotInit;
  }

  const size_t old_capacity = table->capacity;
  const size_t new_capacity = scale * table->capacity;

  void *temp = (void *)realloc(table->entries, new_capacity * table->type_size);

  if (temp == NULL) {
    logger_add(LoggerFlag_Error,
               "Couldn't expand hash table '%s' from %lu to %lu.", table->label,
               table->capacity, new_capacity);
    return HashTableStatus_AllocFail;
  }

  table->entries = temp;
  table->capacity = new_capacity;

  hsht_rehash(table, old_capacity);

  return HashTableStatus_Success;
}

void *hsht_find(HashTable *table, const void *key, size_t *real_index) {

  size_t start = table->generator(key) % table->capacity;
  size_t index = start;

  // DEBUG
  if (strcmp(table->label, "RenderPipeline") == 0)
    printf("start: %lu \n", start);

  while (table->get_occupied((void *)(char *)table->entries +
                             (index * table->type_size))) {

    // DEBUG
    if (strcmp(table->label, "RenderPipeline") == 0)
      printf("occupied \n");

    void *current_entry =
        (void *)(char *)table->entries + (index * table->type_size);

    // DEBUG
    if (strcmp(table->label, "RenderPipeline") == 0)
      printf("comparator result: %d\n", table->comparator(key, current_entry));

    if (table->comparator(key, current_entry)) {
      if (real_index)
        *real_index = index;
      return current_entry;
    }

    index = (index + 1) % table->capacity;
    if (index == start)
      break;
  }

  return NULL;
}

void *hsht_new_entry(HashTable *table, const void *key,
                     const HashTableNewFlag flag) {

  if (table->occupied_list.length >= table->capacity * 0.75) {
    if (flag & HashTableNewFlag_FixedCapacity) {
      logger_add(LoggerFlag_Error,
                 "Unable to generate new entry in Hash table '%s' has a fixed "
                 "capacity of %lu for %lu occupied buckets.",
                 table->label, table->capacity, table->occupied_list.length);
      return NULL;
    } else if (hsht_expand(table->entries, 2) != HashTableStatus_Success)
      return NULL;
  }

  size_t start = table->generator(key) % table->capacity;
  size_t index = start;

  while (table->get_occupied((void *)(char *)(table->entries) +
                             (index * table->type_size))) {

    void *current = (char *)(table->entries) + index * table->type_size;
    if (table->comparator(key, current))
      return current;

    index = (index + 1) % table->capacity;
    if (index == start)
      return NULL;
  }

  void *entry = (void *)((char *)(table->entries) + index * table->type_size);

  hsht_register_occupied_entry(table, key, entry);

  memset(entry, 0, table->type_size);

  if (table->set_occupied)
    table->set_occupied(entry, true);

  return entry;
}

HashTableStatus hsht_remove_entry(HashTable *table, const void *key) {

  void *result = hsht_find(table, key, NULL);

  if (result == NULL)
    return HashTableStatus_UnfoundEntry;

  // DEBUG
  if (strcmp(table->label, "RenderPipeline") == 0)
    printf("remove result: %p\n", result);

  DynamicListStatus unregister = hsht_unregister_occupied_entry(table, key);

  memset(result, 0, table->type_size);

  if (table->set_occupied)
    table->set_occupied(result, false);

  return HashTableStatus_Success;
}

HashTableStatus hsht_empty(void *entries, size_t *length, size_t type_size,
                           const char *label) {

  memset(entries, 0, *length * type_size);

  return HashTableStatus_Success;
}

HashTableStatus hsht_destroy(void **entries, size_t *capacity, size_t *length,
                             const char *label) {

  *capacity = 0;
  *length = 0;

  free(*entries);
  *entries = NULL;

  return HashTableStatus_Success;
}

DynamicListStatus hsht_register_occupied_entry(HashTable *table,
                                               const void *key, void *entry) {

  return dyli_insert(
      (void *)&table->occupied_list.entries, &table->occupied_list.capacity,
      &table->occupied_list.length, sizeof(HashTableOccupiedSlot),
      &(HashTableOccupiedSlot){
          .key = key,
          .entry = entry,
      },
      1, table->label);
}

DynamicListStatus hsht_unregister_occupied_entry(HashTable *table,
                                                 const void *key) {

  for (size_t i = 0; i < table->occupied_list.length; i++) {
    if (table->occupied_list.entries[i].key == key)
      return dyli_remove_at_index(
          (void *)table->occupied_list.entries, &table->occupied_list.length,
          sizeof(HashTableOccupiedSlot), i, table->label);
  }

  return DynamicListStatus_UnfoundEntry;
}
