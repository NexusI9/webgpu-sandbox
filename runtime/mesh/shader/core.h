#ifndef _MESH_SHADER_C_
#define _MESH_SHADER_C_

#include "../backend/ssbo.h"
#include "../runtime/camera/camera.h"
#include "../runtime/mesh/core.h"
#include "../runtime/shader/shader.h"
#include "../runtime/viewport/viewport.h"

/* custom */
void mesh_shader_create(Mesh *, const ShaderCreateDescriptor *);
void mesh_shader_create_fixed(Mesh *, const ShaderCreateDescriptor *);

/* builtins*/
void mesh_shader_create_shadow(Mesh *);
void mesh_shader_create_wireframe(Mesh *);
void mesh_shader_create_solid(Mesh *);

Shader *mesh_shader(Mesh *, const MeshShader);

void mesh_shader_set_active(Mesh *, const MeshShader);

void mesh_shader_build_mvp(Mesh *, const MeshShader, SSBOManager *, Camera *,
                           Viewport *, bool);

#endif
