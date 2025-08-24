#ifndef _MESH_SHADER_C_
#define _MESH_SHADER_C_

#include "../runtime/camera/camera.h"
#include "../runtime/mesh/core.h"
#include "../runtime/shader/shader.h"
#include "../runtime/viewport/viewport.h"

typedef void (*mesh_create_dynamic_shader_callback)(Mesh *);

/* custom */
void mesh_shader_create(Mesh *, const ShaderCreateDescriptor *);
void mesh_shader_create_fixed(Mesh *, const ShaderCreateDescriptor *);

/* builtins*/
void mesh_shader_create_shadow(Mesh *);
void mesh_shader_create_wireframe(Mesh *);
void mesh_shader_create_solid(Mesh *);

Shader *mesh_shader(Mesh *, const MeshShader);

void mesh_shader_set_active(Mesh *, const MeshShader);

void mesh_shader_update_mvp(Mesh *, const MeshShader, Camera *, Viewport *);

void mesh_shader_build_mvp(Mesh *, const MeshShader, Camera *, Viewport *, bool);

typedef void (*mesh_shader_bind_views_callback)(Mesh *, Camera *, Viewport *);

#endif
