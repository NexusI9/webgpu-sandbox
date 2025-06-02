#ifndef _MESH_SHADER_C_
#define _MESH_SHADER_C_

#include "../shader/shader.h"
#include "./core.h"

void mesh_init_shadow_shader(Mesh *);
void mesh_init_wireframe_shader(Mesh *);

Shader *mesh_shader_texture(Mesh *);
Shader *mesh_shader_shadow(Mesh *);
Shader *mesh_shader_wireframe(Mesh *);
Shader *mesh_shader_solid(Mesh *);

#endif
