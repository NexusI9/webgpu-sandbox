#ifndef _MATERIAL_WIREFRAME_H_
#define _MATERIAL_WIREFRAME_H_
#include "core.h"

void material_wireframe_bind_views(Mesh *, Camera *, Viewport *, uint8_t);
void material_wireframe_double_sided(Mesh *mesh);
#endif
