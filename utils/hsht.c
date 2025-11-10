#include "hsht.h"
#include "backend/logger.h"
#include "utils/dyli.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

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
  table->get_key = desc->get_key_callback;

  return HashTableStatus_Success;
}

HashTableStatus hsht_expand(HashTable *table, const size_t scale) {

  // DEBUG
  printf("Expanding table\n");

  if (table->capacity == 0) {
    logger_add(LoggerFlag_Error,
               "Hash table '%s' has a capaicty of 0. Make sure it's been "
               "initialized corectly.\n",
               table->label);
    return HashTableStatus_NotInit;
  }

  void *old_entries = table->entries;
  const size_t old_capacity = table->capacity;
  const size_t new_capacity = scale * table->capacity;

  void *temp = (void *)calloc(new_capacity, table->type_size);

  if (temp == NULL) {
    logger_add(LoggerFlag_Error,
               "Couldn't expand hash table '%s' from %lu to %lu.", table->label,
               table->capacity, new_capacity);
    return HashTableStatus_AllocFail;
  }

  if (!table->get_key) {
    logger_add(LoggerFlag_Error,
               "Hash table '%s' is missing get_key "
               "callback, unable to expand it.",
               table->label);
    free(temp);
    return HashTableStatus_MissingCallback;
  }

  table->entries = temp;
  table->capacity = new_capacity;
  table->length = 0;

  // rehash
  for (size_t i = 0; i < old_capacity; i++) {

    char *old_entry = (char *)old_entries + i * table->type_size;

    if (table->get_occupied((void *)old_entry)) {

      void *old_entry_key = table->get_key(old_entry);

      void *new_entry =
          hsht_new_entry(table, old_entry_key, HashTableNewFlag_None);

      if (!new_entry) {
        logger_add(LoggerFlag_Warning,
                   "While expanding hash table '%s', unable to get new entry.",
                   table->label);
        continue;
      }

      memcpy(new_entry, old_entry, table->type_size);
    }
  }

  free(old_entries);

  return HashTableStatus_Success;
}

void *hsht_find(HashTable *table, const void *key, size_t *real_index) {

  size_t start = table->generator(key) % table->capacity;
  size_t index = start;

  while (table->get_occupied((void *)(char *)table->entries +
                             (index * table->type_size))) {

    void *current_entry =
        (void *)(char *)table->entries + (index * table->type_size);

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

/**
   Return a hash-table slot based on the provided key
 */
void *hsht_new_entry(HashTable *table, const void *key,
                     const HashTableNewFlag flag) {

  if (table->length >= table->capacity * 0.75) {
    if (flag & HashTableNewFlag_FixedCapacity) {
      logger_add(LoggerFlag_Error,
                 "Unable to generate new entry in Hash table '%s' has a fixed "
                 "capacity of %lu for %lu occupied buckets.",
                 table->label, table->capacity, table->length);
      return NULL;
    } else if (hsht_expand(table, 2) != HashTableStatus_Success)
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

  // DEBUG
  printf("%p => %lu  => %p\n", key, start,
         (char *)(table->entries) + (index * table->type_size));

  memset(entry, 0, table->type_size);

  if (table->set_occupied)
    table->set_occupied(entry, true);

  table->length++;

  return entry;
}

HashTableStatus hsht_remove_entry(HashTable *table, const void *key) {

  void *result = hsht_find(table, key, NULL);

  if (result == NULL)
    return HashTableStatus_UnfoundEntry;

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
