#include "registry.h"
#include "backend/logger.h"
#include <stdio.h>

RegEntry g_reg[REG_MAX_OBJECTS] = {0};
reg_id_t g_reg_id = 1;

static inline reg_id_t gen_id();

reg_id_t gen_id() { return g_reg_id++; }

/**
   Add object pointer to register and return the id
 */
reg_id_t reg_register(void *ptr, RegEntryType type) {
  if (g_reg_id == REG_MAX_OBJECTS) {
    logger_add(LoggerFlag_Error, "Cannot add more objects to registry.");
    return 0;
  }

  // assign object to global register
  reg_id_t id = gen_id();
  g_reg[id].ptr = ptr;
  g_reg[id].type = type;
  g_reg[id].id = id;

  return id;
}
