#ifndef _UBO_SYSTEM_H_
#define _UBO_SYSTEM_H_

#include "backend/renderer/core.h"
#include "backend/ubo.h"

EXTERN_C_BEGIN

void ubo_system_register_draw_callback(UBOManager *, Renderer *);

void ubo_system_draw_callback(Renderer *, void *);

EXTERN_C_END

#endif
