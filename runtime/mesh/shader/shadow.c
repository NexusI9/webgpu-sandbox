#include "shadow.h"
#include "../utils/system.h"
#include "./core.h"

/**
   Bind a specific point light view to the mesh's shadow shader
   The function is called during the scene shadow updating process
   As to provide to the shadow shader each lights views.

   Note that the view matrix shall be combination of the [projection view] *
   [light view] already multiplied together as there is currently no need to
   upload separate views in the shader.
 */
void mesh_shader_shadow_build_mvp(Mesh *mesh, SSBOManager *ssbo_manager) {

  // retrieve the model-view-projection binding index from the pipeline
  Shader *shader = mesh_shader(mesh, MeshShader_Shadow);

  ShaderBindGroupUniformEntry entries[2] = {
      // viewport x cam
      {
          .binding = 0,
          .buffer = ssbo_buffer_handle(ssbo_manager, SSBOType_ViewShadow),
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
