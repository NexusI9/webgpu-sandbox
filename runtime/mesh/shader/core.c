#include "core.h"
#include "../backend/renderer/scene/std_pipeline/std_pipeline.h"
#include "../utils/math.h"

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

Shader *mesh_shader(Mesh *mesh, const MeshShader shader) {
  return &mesh->shader.standard[shader];
}

void mesh_shader_set_active(Mesh *mesh, const MeshShader shader) {
  mesh->shader.active = &mesh->shader.standard[shader];
}

/**
   Init mesh shadow shader.
   By default all mesh have a shadow shader to generate shadow map
   during the bind light process we will generate the depth map since that's
   where we get out scene lights.

   The init shadow shader doesn't belong to the material API as it is a
   necessary component set by default on mesh creation.
 */
void mesh_shader_create_shadow(Mesh *mesh) {

  // import shadow shader
  Shader *shadow_shader = mesh_shader(mesh, MeshShader_Shadow);
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
void mesh_shader_create_wireframe(Mesh *mesh) {

  Shader *wireframe_shader = mesh_shader(mesh, MeshShader_Wireframe);

  // skip if already created
  if (wireframe_shader->name != NULL) {
    VERBOSE_INFO(
        "Wireframe shader for %s is already created, skip shader creation.",
        mesh->name);
    return;
  }

  // create shader
  shader_create(wireframe_shader,
                &(ShaderCreateDescriptor){
                    .pipeline = std_pipeline(PipelineType_Line),
                    .label = "Mesh wireframe shader",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "Mesh wireframe shader",
                });

  shader_update_uniform_data(wireframe_shader, 1, 0,
                             &(color){randf(), randf(), randf(), 1.0f});
}

/**
   Initialize solid shader
 */
void mesh_shader_create_solid(Mesh *mesh) {

  Shader *solid_shader = mesh_shader(mesh, MeshShader_Solid);

  // create shader
  shader_create(solid_shader, &(ShaderCreateDescriptor){
                                  .pipeline = std_pipeline(PipelineType_Solid),
                                  .label = "Mesh solid shader",
                                  .device = mesh->device,
                                  .queue = mesh->queue,
                                  .name = "Mesh solid shader",
                              });
}

/**
   Set texture shader.
 */
void mesh_shader_create(Mesh *mesh, const ShaderCreateDescriptor *desc) {
  // create texture shader as default
  shader_create(mesh_shader(mesh, MeshShader_Texture), desc);

  // also initialise the reflection shader (basically a copy of the texture)
  shader_create(mesh_shader(mesh, MeshShader_Reflection),
                &(ShaderCreateDescriptor){
                    .pipeline = std_pipeline(PipelineType_Reflection),
                    .label = "Mesh Reflection shader",
                    .name = "Mesh Reflection shader",
                    .device = mesh->device,
                    .queue = mesh->queue,
                });

  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Texture);
}

/**
   Set texture shader.
 */
void mesh_shader_create_fixed(Mesh *mesh, const ShaderCreateDescriptor *desc) {
  // alias to shader_create
  shader_create(mesh_shader(mesh, MeshShader_Fixed), desc);
  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Fixed);
}

/**
   Build Mesh, Camera and Projection matrix to a given mesh shader.
   It replaces the initial bound values by the ones provided by the scene
   (active camera matrix, viewport data).
   For Views uniform we actually link to the scene SSBO to allow
   queue and batch update.

   Additionally it also add the relative callbacks and trigger ensuring the mesh
   update their mvp on camera move and mesh translation.

   Note that the binding process follows a
   fixed convention of order, meaning one shall ensure the shader actually fits
   the bellow binding order:
   - Binding 0: Viewport projection matrix
   - Binding 1: Camera matrix
   - Binding 2: Model matrix

   This function is primarily used when a mesh is firstly added to the scene.
 */
void mesh_shader_build_mvp(Mesh *mesh, const MeshShader shader_type,
                           SSBOManager *ssbo_manager) {

  // retrieve the model-view-projection binding index from the pipeline
  Shader *shader = mesh_shader(mesh, shader_type);
  const PipelineBindingMVP *mvp = shader->pipeline->bindings.mvp;

  ShaderBindGroupUniformEntry entries[3] = {
      // viewport
      {
          .binding = mvp->projection,
          .buffer = ssbo_buffer_handle(ssbo_manager, SSBOType_Projection),
          .offset = 0, // active vewport index
      },
      // camera
      {
          .binding = mvp->view,
          .buffer = ssbo_buffer_handle(ssbo_manager, SSBOType_View),
          .offset = 0, // active camera index
      },
      // model
      {
          .binding = mvp->model,
          .buffer = ssbo_buffer_handle(ssbo_manager, SSBOType_Mesh),
          .offset = mesh->ssbo_slot.id,
      },
  };

  for (size_t i = 0; i < 3; i++) {
    ShaderBindGroupUniformEntry *entry = &entries[i];
    shader_update_uniform_buffer(shader, mvp->group, entry->binding,
                                 entry->buffer, entry->offset,
                                 ShaderBufferLifetime_Release);
  }
}

/**
   Bind a specific point light view to the mesh's shadow shader
   The function is called during the scene shadow updating process
   As to provide to the shadow shader each lights views.

   Note that the view matrix shall be combination of the [projection view] *
   [light view] already multiplied together as there is currently no need to
   upload separate views in the shader.

   Build settings mostly used for shadow and reflection shader.
 */
void mesh_shader_build_mp(Mesh *mesh, const MeshShader shader_type,
                          SSBOManager *ssbo_manager,
                          const SSBOType ssbo_view_type) {

  // retrieve the model-view-projection binding index from the pipeline
  Shader *shader = mesh_shader(mesh, shader_type);

  ShaderBindGroupUniformEntry entries[2] = {
      // viewport x cam
      {
          .binding = 0,
          .buffer = ssbo_buffer_handle(ssbo_manager, ssbo_view_type),
          .offset = 0,
      },
      // model
      {
          .binding = 1,
          .buffer = ssbo_buffer_handle(ssbo_manager, SSBOType_Mesh),
          .offset = mesh->ssbo_slot.id,
      },
  };

  for (size_t i = 0; i < 2; i++) {
    ShaderBindGroupUniformEntry *entry = &entries[i];
    shader_update_uniform_buffer(shader, 0, entry->binding, entry->buffer,
                                 entry->offset, ShaderBufferLifetime_Release);
  }
}
