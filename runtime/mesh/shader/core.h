#ifndef _MESH_SHADER_C_
#define _MESH_SHADER_C_

#include "backend/ubo.h"
#include "runtime/camera/camera.h"
#include "runtime/mesh/core.h"
#include "runtime/shader/core.h"
#include "runtime/shader/shader.h"
#include "runtime/viewport/viewport.h"

EXTERN_C_BEGIN

/* custom */
MeshStatus mesh_shader_create(Mesh *, const ShaderCreateDescriptor *);
MeshStatus mesh_shader_create_fixed(Mesh *, const ShaderCreateDescriptor *);

/* builtins */
MeshStatus mesh_shader_create_standard(Mesh *, const MeshShader);

Shader *mesh_shader(Mesh *, const MeshShader);

void mesh_shader_build_mvp(Mesh *, const MeshShader, UBOManager *);
void mesh_shader_build_mp(Mesh *, const MeshShader, UBOManager *,
                          const UBOType);

EXTERN_C_END

#endif
