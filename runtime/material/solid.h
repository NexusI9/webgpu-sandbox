#ifndef _MATERIAL_SOLID_H_
#define _MATERIAL_SOLID_H_
#include "core.h"

void material_solid_bind_views(Mesh *, Camera *, Viewport *, uint8_t);
void material_solid_double_sided(Mesh *mesh);

#endif
