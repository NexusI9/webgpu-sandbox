#ifndef _MESH_SHADER_C_
#define _MESH_SHADER_C_

#include "../shader/shader.h"
#include "./core.h"

typedef Shader *(*mesh_get_shader_callback)(Mesh *);
typedef void (*mesh_create_dynamic_shader_callback)(Mesh *);

void mesh_create_shadow_shader(Mesh *);
void mesh_create_wireframe_shader(Mesh *);

Shader *mesh_shader_texture(Mesh *);
Shader *mesh_shader_shadow(Mesh *);
Shader *mesh_shader_wireframe(Mesh *);
Shader *mesh_shader_solid(Mesh *);

#endif
