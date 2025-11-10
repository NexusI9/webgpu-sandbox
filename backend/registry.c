#include "registry.h"
#include "backend/logger.h"
#include <stdio.h>

RegEntry g_reg[REG_MAX_OBJECTS] = {0};
reg_id_t g_reg_id = 1;

/**
   Add object pointer to register with a given id, id can obtained with
   reg_new_id()
 */
reg_id_t reg_register(void *ptr, RegEntryType type) {

  const reg_id_t id = reg_new_id();

  // assign object to global register
  g_reg[id].ptr = ptr;
  g_reg[id].type = type;
  g_reg[id].id = id;

  return id;
}
