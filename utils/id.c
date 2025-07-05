#include "id.h"

static id_t global_id = 1;

id_t gen_id() { return global_id++; }
