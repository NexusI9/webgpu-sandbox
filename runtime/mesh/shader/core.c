#include "core.h"

#include <stddef.h>

#include "backend/logger.h"
#include "backend/ssbo.h"
#include "backend/std_pipeline/core.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "utils/color.h"

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

// Standard shaders automatically created on mesh creation and based on render
// draw mode (solid/ wireframe...)
static const struct {
  const char *name;
  const RenderPipelineType pipeline_type;
} standard_shader_map[] = {
    [MeshShader_Shadow] =
        {
            .name = "Mesh Std Shader - Shadow",
            .pipeline_type = RenderPipelineType_Shadow,
        },
    [MeshShader_Solid] =
        {
            .name = "Mesh Std Shader - Solid",
            .pipeline_type = RenderPipelineType_Solid,
        },
    [MeshShader_Wireframe] =
        {
            .name = "Mesh Std Shader - Wireframe",
            .pipeline_type = RenderPipelineType_Line,
        },
    [MeshShader_Outline] =
        {
            .name = "Mesh Std Shader - Outline",
            .pipeline_type = RenderPipelineType_Outline,
        },
    [MeshShader_Stencil] =
        {
            .name = "Mesh Std Shader - Stencil",
            .pipeline_type = RenderPipelineType_Stencil,
        },
};

/**
   Init mesh standard shader.
   By default all dynamic meshes have a outline/shadow/solid shader to generate
   shadow map during the bind light process we will generate the depth map since
   that's where we get out scene lights.
 */
MeshStatus mesh_shader_create_standard(Mesh *mesh,
                                       const MeshShader shader_type) {

  if (shader_type < 2 || shader_type > MESH_STD_SHADER_COUNT) {
    logger_add(
        LoggerFlag_Warning,
        "Attempting to create a standard mesh shader for '%s' with an invalid "
        "index (%d)",
        mesh->name, shader_type);
    return MeshStatus_InvalidShaderIndex;
  }

  Shader *shader = mesh_shader(mesh, shader_type);

  if (shader->name != NULL) {
    logger_add(
        LoggerFlag_Info,
        "Shader '%s' for mesh '%s' is already created, skip shader creation.",
        standard_shader_map[shader_type].name, mesh->name);
    return MeshStatus_AlreadyCreated;
  }

  shader_create(shader, &(ShaderCreateDescriptor){
                            .pipeline = std_render_pipeline(
                                standard_shader_map[shader_type].pipeline_type),
                            .name = standard_shader_map[shader_type].name,
                        });

  return MeshStatus_Success;
}

/**
   Set texture shader.
 */
MeshStatus mesh_shader_create(Mesh *mesh, const ShaderCreateDescriptor *desc) {

  Shader *texture_shader = mesh_shader(mesh, MeshShader_Texture);

  if (texture_shader->name != NULL) {
    logger_add(
        LoggerFlag_Info,
        "Texture shader for '%s' is already created, skip shader creation.",
        mesh->name);
    return MeshStatus_AlreadyCreated;
  }

  // create texture shader as default
  shader_create(texture_shader, desc);

  // also initialise the reflection shader (basically a copy of the texture)
  shader_create(
      mesh_shader(mesh, MeshShader_Reflection),
      &(ShaderCreateDescriptor){
          .pipeline = std_render_pipeline(RenderPipelineType_Reflection),
          .name = "Mesh Reflection shader",
      });

  return MeshStatus_Success;
}

/**
   Set texture shader.
 */
MeshStatus mesh_shader_create_fixed(Mesh *mesh,
                                    const ShaderCreateDescriptor *desc) {

  Shader *fixed_shader = mesh_shader(mesh, MeshShader_Texture);

  if (fixed_shader->name != NULL) {
    logger_add(
        LoggerFlag_Info,
        "Fixed shader for '%s' is already created, skip shader creation.",
        mesh->name);
    return MeshStatus_AlreadyCreated;
  }

  // alias to shader_create
  shader_create(mesh_shader(mesh, MeshShader_Fixed), desc);

  return MeshStatus_Success;
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
          .buffer = ssbo_buffer_handle(ssbo_manager, SSBOType_Viewport),
          .offset = 0, // active vewport index
      },
      // camera
      {
          .binding = mvp->view,
          .buffer = ssbo_buffer_handle(ssbo_manager, SSBOType_Camera),
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
