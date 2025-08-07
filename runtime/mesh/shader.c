#include "shader.h"
#include "../backend/renderer/renderer.h"
#include "../material/material.h"
#include "core.h"

/**
   ▗▖  ▗▖ ▗▄▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖
   ▐▛▚▖▐▌▐▌ ▐▌ █    █  ▐▌  ▐▌▐▌
   ▐▌ ▝▜▌▐▛▀▜▌ █    █  ▐▌  ▐▌▐▛▀▀▘
   ▐▌  ▐▌▐▌ ▐▌ █  ▗▄█▄▖ ▝▚▞▘ ▐▙▄▄▖

    ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▄ ▗▄▄▄▖▗▄▄▖  ▗▄▄▖
   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌   ▐▌ ▐▌▐▌
    ▝▀▚▖▐▛▀▜▌▐▛▀▜▌▐▌  █▐▛▀▀▘▐▛▀▚▖ ▝▀▚▖
   ▗▄▄▞▘▐▌ ▐▌▐▌ ▐▌▐▙▄▄▀▐▙▄▄▖▐▌ ▐▌▗▄▄▞▘

 */

/**
   Return mesh default shader
 */
Shader *mesh_shader_texture(Mesh *mesh) { return &mesh->shader.standard[MeshShader_Texture]; }

/**
   Return mesh shadow shader
 */
Shader *mesh_shader_shadow(Mesh *mesh) { return &mesh->shader.standard[MeshShader_Shadow]; }

/**
   Return mesh wireframe shader
 */
Shader *mesh_shader_wireframe(Mesh *mesh) { return &mesh->shader.standard[MeshShader_Wireframe]; }

/**
   Return mesh solid shader
 */
Shader *mesh_shader_solid(Mesh *mesh) { return &mesh->shader.standard[MeshShader_Solid]; }

/**
   Return mesh override shader
   Primarily used for fixed layer during the scene build/draw process.
 */
Shader *mesh_shader_override(Mesh *mesh) { return mesh->shader.override; }

/**
   Init mesh shadow shader.
   By default all mesh have a shadow shader to generate shadow map
   during the bind light process we will generate the depth map since that's
   where we get out scene lights.

   The init shadow shader doesn't belong to the material API as it is a
   necessary component set by default on mesh creation.
 */
void mesh_create_shadow_shader(Mesh *mesh) {

  // import shadow shader
  Shader *shadow_shader = mesh_shader_shadow(mesh);
  shader_create(shadow_shader,
                &(ShaderCreateDescriptor){
                    .pipeline = std_pipeline(PipelineType_Shadow),
                    .label = "Mesh shadow shader",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "Mesh shadow shader",
                });
}

/**
   Initialize Wireframe shader.
   Wireframe use a second vertex and index buffer (buffer.wireframe), since
   wireframe require to draw lines for each edges, however lines are basically
   rendered as very thin quads, which requires to duplicate each vertex once.

   The init wireframe shader doesn't belong to the material API as it is a
   necessary component set by default on mesh creation.

   Overall process:
     1. Isolate unique edges
     2. Create lines for each pair
     3. Upload data to GPU buffer
     4. Create wireframe shader
 */
void mesh_create_wireframe_shader(Mesh *mesh) {

  Shader *wireframe_shader = mesh_shader_wireframe(mesh);

  // skip if already created
  if (wireframe_shader->name != NULL)
    return;

  // create shader
  shader_create(wireframe_shader,
                &(ShaderCreateDescriptor){
                    .pipeline = &g_std_pipelines[PipelineType_Line],
                    .label = "Mesh wireframe shader",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "Mesh wireframe shader",
                });
}

/**
   Initialize solid shader
 */
void mesh_create_solid_shader(Mesh *mesh) {

  Shader *solid_shader = mesh_shader_solid(mesh);

  // create shader
  shader_create(solid_shader,
                &(ShaderCreateDescriptor){
                    .pipeline = &g_std_pipelines[PipelineType_Solid],
                    .label = "Mesh solid shader",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "Mesh solid shader",
                });
}

/**
   Override shader allow to direct toward another shader for any rendering type.
   This can become handy for gizmo if they need to appear as "wireframe" instead
   of solid. Shader Override often comes hand in hand with Topology Override.
   Override basically means:
   "I want you to use this topology and shader no matter the rendering mode"
   (wireframe/ solid/ textured..)
 */
void mesh_shader_set_override(Mesh *mesh, Shader *shader) {
  mesh->shader.override = shader;
}
