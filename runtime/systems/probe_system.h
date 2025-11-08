#ifndef _PROBE_SYSTEM_H_
#define _PROBE_SYSTEM_H_

#include "backend/renderer/core.h"
#include "backend/renderer/reflection/core.h"
#include "runtime/probe/core.h"

EXTERN_C_BEGIN

void probe_system_init_reflection_pass(ProbeList *, Renderer *);

EXTERN_C_END

#endif
