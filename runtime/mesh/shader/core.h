#ifndef _MESH_SHADER_C_
#define _MESH_SHADER_C_

#include "../runtime/mesh/core.h"
#include "../runtime/shader/shader.h"
#include "../runtime/camera/camera.h"
#include "../runtime/viewport/viewport.h"

typedef Shader *(*mesh_get_shader_callback)(Mesh *);
typedef void (*mesh_create_dynamic_shader_callback)(Mesh *);

/* custom */
void mesh_shader_create(Mesh *, const ShaderCreateDescriptor *);
void mesh_shader_create_fixed(Mesh *, const ShaderCreateDescriptor *);
/* builtins*/
void mesh_shader_create_shadow(Mesh *);
void mesh_shader_create_wireframe(Mesh *);
void mesh_shader_create_solid(Mesh *);

void mesh_shader_set_active(Mesh *, const MeshShader);

Shader *mesh_shader_texture(Mesh *);
Shader *mesh_shader_shadow(Mesh *);
Shader *mesh_shader_wireframe(Mesh *);
Shader *mesh_shader_solid(Mesh *);
Shader *mesh_shader_fixed(Mesh *); // for fixed mesh only
Shader *mesh_shader_active(Mesh *);


/* Below function use ACTIVE shader as target */
void mesh_shader_bind_views(Mesh *, Camera *, Viewport *);

#endif
