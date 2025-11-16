#ifndef _EXAMPLE_GLASS_H_
#define _EXAMPLE_GLASS_H_

#include <stdbool.h>

#include "backend/renderer/core.h"
#include "runtime/engine/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/scene.h"

void example_glass_box(Engine *);
void example_glass_probe_grid(Engine *, bool);
void example_glass_probe_plane(Engine *, bool);

#endif
