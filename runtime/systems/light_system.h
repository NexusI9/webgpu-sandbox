#ifndef _LIGHT_SYSTEM_H_
#define _LIGHT_SYSTEM_H_

#include "backend/renderer/core.h"
#include "runtime/light/list.h"
#include "runtime/mesh/core.h"
#include "utils/defines.h"

EXTERN_C_BEGIN

void light_system_init_shadow_map(LightList *, Renderer *);

EXTERN_C_END
#endif
