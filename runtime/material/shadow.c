#include "shadow.h"


/**
   Bind a specific point light view to the mesh's shadow shader
   The function is called during the scene shadow updating process
   As to provide to the shadow shader each lights views.

   Note that the view matrix shall be combination of the [projection view] *
   [light view] already multiplied together as there is currently no need to
   upload separate views in the shader.
 */

void material_shadow_bind_views(Mesh *mesh, mat4 *view) {

  MeshUniform uModel = mesh_uniform_model(mesh);

  shader_add_uniform(
      mesh_shader_shadow(mesh),
      &(ShaderCreateUniformDescriptor){
          .group_index = 0,
          .entry_count = 2,
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entries =
              (ShaderBindGroupUniformEntry[]){
                  {
                      .binding = 0,
                      .data = view,
                      .size = sizeof(mat4),
                      .offset = 0,
                  },
                  {
                      .binding = 1,
                      .data = &uModel,
                      .size = sizeof(MeshUniform),
                      .offset = 0,
                  },
              },
      });
}

void material_shadow_set_cullmode(Mesh *mesh, WGPUCullMode mode) {

  pipeline_set_primitive(shader_pipeline(mesh_shader_shadow(mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_Back,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });
}


/**
   Clear the shadow shader bind groups of mesh
 */
void material_shadow_clear_bindings(Mesh *mesh) {
  shader_bind_group_clear(mesh_shader_shadow(mesh));
}


