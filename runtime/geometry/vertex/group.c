#include "group.h"
#include "../utils/hash.h"
#include "index.h"
#include <stdint.h>
#include <string.h>


/**
▗▖  ▗▖▗▄▄▄▖▗▄▄▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖     ▗▄▄▖▗▄▄▖  ▗▄▖ ▗▖ ▗▖▗▄▄▖
▐▌  ▐▌▐▌   ▐▌ ▐▌ █  ▐▌    ▝▚▞▘     ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌
▐▌  ▐▌▐▛▀▀▘▐▛▀▚▖ █  ▐▛▀▀▘  ▐▌      ▐▌▝▜▌▐▛▀▚▖▐▌ ▐▌▐▌ ▐▌▐▛▀▘
 ▝▚▞▘ ▐▙▄▄▖▐▌ ▐▌ █  ▐▙▄▄▖▗▞▘▝▚▖    ▝▚▄▞▘▐▌ ▐▌▝▚▄▞▘▝▚▄▞▘▐▌
*/

static int vertex_group_expand(VertexGroup *);

int vertex_group_expand(VertexGroup *group) {

  size_t new_capacity = 2 * group->capacity;
  vindex_t *temp =
      (vindex_t *)realloc(group->entries, new_capacity * sizeof(vindex_t));

  if (temp == NULL) {
    perror("Couldn't expand new vertex group\n");
    return VERTEX_GROUP_ALLOC_FAIL;
  }

  group->entries = temp;
  group->capacity = new_capacity;

  return VERTEX_GROUP_SUCCESS;
}

int vertex_group_create(VertexGroup *group, size_t capacity, const char *name) {

  group->entries = calloc(capacity, sizeof(vindex_t));
  group->length = 0;
  group->capacity = capacity;
  group->name = strdup(name);

  if (group->entries == NULL || group->name == NULL) {
    perror("Couldn't create new vertex group\n");
    group->capacity = 0;
    return VERTEX_GROUP_ALLOC_FAIL;
  }

  return VERTEX_GROUP_SUCCESS;
}

VertexGroup *vertex_group_insert(VertexGroup *group, vindex_t *index_list,
                                 size_t length) {

  while (group->length + length > group->capacity) {
    if (vertex_group_expand(group) != VERTEX_GROUP_SUCCESS) {
      perror("Couldn't insert new vertex group value\n");
      return NULL;
    }
  }

  memcpy(&group->entries[group->length], index_list, length * sizeof(vindex_t));
  group->length += length;

  return group;
}

void vertex_group_free(VertexGroup *group) {
  free(group->entries);
  group->entries = NULL;

  free(group->name);
  group->name = NULL;

  group->length = 0;
  group->capacity = 0;
}

/**

▗▖  ▗▖▗▄▄▄▖▗▄▄▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖     ▗▄▄▖▗▄▄▖  ▗▄▖ ▗▖ ▗▖▗▄▄▖      ▗▄▄▖▗▄▄▄▖▗▄▄▄▖
▐▌  ▐▌▐▌   ▐▌ ▐▌ █  ▐▌    ▝▚▞▘     ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌    ▐▌   ▐▌     █
▐▌  ▐▌▐▛▀▀▘▐▛▀▚▖ █  ▐▛▀▀▘  ▐▌      ▐▌▝▜▌▐▛▀▚▖▐▌ ▐▌▐▌ ▐▌▐▛▀▘      ▝▀▚▖▐▛▀▀▘  █
 ▝▚▞▘ ▐▙▄▄▖▐▌ ▐▌ █  ▐▙▄▄▖▗▞▘▝▚▖    ▝▚▄▞▘▐▌ ▐▌▝▚▄▞▘▝▚▄▞▘▐▌       ▗▄▄▞▘▐▙▄▄▖  █

 */

static int vertex_group_set_expand(VertexGroupSet *);
static void vertex_group_set_rehash(VertexGroupSet *);

void vertex_group_set_rehash(VertexGroupSet *set) {}

int vertex_group_set_expand(VertexGroupSet *set) {

  size_t new_capacity = 2 * set->capacity;
  VertexGroup *temp =
      (VertexGroup *)realloc(set->entries, new_capacity * sizeof(VertexGroup));

  if (temp == NULL) {
    perror("Couldn't expand new vertex group set\n");
    return VERTEX_GROUP_ALLOC_FAIL;
  }

  set->entries = temp;
  set->capacity = new_capacity;

  return VERTEX_GROUP_SUCCESS;
}

int vertex_group_set_create(VertexGroupSet *set, size_t capacity) {

  set->entries = calloc(capacity, sizeof(VertexGroup));
  set->length = 0;
  set->capacity = capacity;

  if (set->entries == NULL) {
    perror("Couldn't create new vertex group set\n");
    set->capacity = 0;
    return VERTEX_GROUP_ALLOC_FAIL;
  }

  return VERTEX_GROUP_SUCCESS;
}

VertexGroup *vertex_group_set_insert(VertexGroupSet *set,
                                     VertexGroup *new_group) {

  if (set->length >= set->capacity * 0.75 &&
      vertex_group_set_expand(set) != VERTEX_GROUP_SUCCESS) {
    return NULL;
  }

  vgroup_hash hash = hash_djb2(new_group->name) % set->capacity;

  // init new vertex group
  VertexGroup *vgroup = &set->entries[hash];

  vgroup_hash init_hash = hash;

  while (true) {
    // if not occupied or names don't match(collision)
    if (vgroup->entries == NULL || strcmp(vgroup->name, new_group->name) == 0)
      break;

    hash = (hash + 1) % set->capacity;
    if (hash == init_hash)
      return NULL;

    vgroup = &set->entries[hash];
  }

  vertex_group_create(vgroup, new_group->length, new_group->name);

  // copy data to new group
  memcpy(vgroup->entries, new_group->entries,
         new_group->length * sizeof(vindex_t));
  vgroup->length = new_group->length;
  vgroup->capacity = new_group->length;

  return vgroup;
}

VertexGroup *vertex_group_set_find(VertexGroupSet *set, vgroup_key key) {
  vgroup_hash hash = hash_djb2(key) % set->capacity;

  vgroup_hash init_hash = hash;
  while (true) {

    VertexGroup *vgroup = &set->entries[hash];

    // if empty slot
    if (vgroup->entries == NULL || vgroup->name == NULL)
      return NULL;

    // if name match key
    if (strcmp(vgroup->name, key) == 0)
      return vgroup;

    // probing
    hash = (hash + 1) % set->capacity;

    if (init_hash == hash)
      return NULL;
  }
}

int vertex_group_set_delete(VertexGroupSet *set, vgroup_key key) {

  vgroup_hash hash = hash_djb2(key) % set->capacity;

  // init new vertex group
  VertexGroup *vgroup = vertex_group_set_find(set, key);

  if (vgroup->entries) {
    vertex_group_free(vgroup);
    set->length--;
    return VERTEX_GROUP_SUCCESS;
  }

  return VERTEX_GROUP_SET_UNFOUND;
}

void vertex_group_set_free(VertexGroupSet *set) {

  // free vertex group
  for (size_t i = 0; i < set->capacity; i++) {
    VertexGroup *vgroup = &set->entries[i];
    if (vgroup->entries)
      vertex_group_free(vgroup);
  }

  // free set
  free(set->entries);
  set->entries = NULL;
  set->capacity = 0;
  set->length = 0;
}
