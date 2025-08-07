#ifndef _MESH_SHADER_UTILS_H_
#define _MESH_SHADER_UTILS_H_

#include "../runtime/mesh/core.h"
#include "../runtime/mesh/shader/shader.h"

void mesh_shader_bind_views(Mesh *, mesh_get_shader_callback, Camera *,
                         Viewport *);

typedef void (*mesh_shader_bind_views_callback)(Mesh *, Camera *, Viewport *);
#endif
