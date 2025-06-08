#ifndef _MATERIAL_CORE_H_
#define _MATERIAL_CORE_H_

#include "../light/light.h"
#include "../mesh/mesh.h"

void material_bind_views(Mesh *, mesh_get_shader_callback, Camera *, Viewport *,
                         uint8_t);

typedef void (*material_bind_views_callback)(Mesh *, Camera *, Viewport *,
                                             uint8_t);
#endif
