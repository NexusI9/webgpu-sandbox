#ifndef _MESH_SHADER_C_
#define _MESH_SHADER_C_

#include "backend/ssbo.h"
#include "runtime/camera/camera.h"
#include "runtime/mesh/core.h"
#include "runtime/shader/core.h"
#include "runtime/shader/shader.h"
#include "runtime/viewport/viewport.h"

/* custom */
MeshStatus mesh_shader_create(Mesh *, const ShaderCreateDescriptor *);
MeshStatus mesh_shader_create_fixed(Mesh *, const ShaderCreateDescriptor *);

/* builtins */
MeshStatus mesh_shader_create_standard(Mesh *, const MeshShader);
MeshStatus mesh_shader_create_shadow(Mesh *);
MeshStatus mesh_shader_create_wireframe(Mesh *);
MeshStatus mesh_shader_create_solid(Mesh *);
MeshStatus mesh_shader_create_outline(Mesh *);

Shader *mesh_shader(Mesh *, const MeshShader);

void mesh_shader_build_mvp(Mesh *, const MeshShader, SSBOManager *);
void mesh_shader_build_mp(Mesh *, const MeshShader, SSBOManager *,
                          const SSBOType);

#endif
