#include "registry.h"
#include <stdio.h>

static id_t g_reg_id = 1;

static inline id_t gen_id();

id_t gen_id() { return g_reg_id++; }

/**
   Add object pointer to register and return the id
 */
id_t reg_register(void *ptr, RegEntryType type) {
  if (g_reg_id == REG_MAX_OBJECTS) {
    perror("Cannot add more objects to registry\n");
    return 0;
  }

  // assign object to global register
  id_t id = gen_id();
  g_reg[id].ptr = ptr;
  g_reg[id].type = type;
  g_reg[id].id = id;

  return id;
}

void *reg_lookup(id_t id) {
  if (id > REG_MAX_OBJECTS) {
    perror("id out of registry bounds\n");
    return NULL;
  }
  return g_reg[id].ptr;
}
