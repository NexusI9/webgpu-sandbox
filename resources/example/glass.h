#ifndef _EXAMPLE_GLASS_H_
#define _EXAMPLE_GLASS_H_

#include <stdbool.h>

#include "runtime/scene/scene.h"
#include "runtime/scene/core.h"

void example_glass_box(Scene *);

void example_glass_probe_grid(Scene *, bool);

void example_glass_probe_plane(Scene *, bool);

#endif
