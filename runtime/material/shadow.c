#include "shadow.h"
#include "../utils/system.h"

/**
   Bind a specific point light view to the mesh's shadow shader
   The function is called during the scene shadow updating process
   As to provide to the shadow shader each lights views.

   Note that the view matrix shall be combination of the [projection view] *
   [light view] already multiplied together as there is currently no need to
   upload separate views in the shader.
 */

void material_shadow_bind_views(Mesh *mesh) {

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
                      .data = (void *)0,
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

void material_shadow_update_views(Mesh *mesh, mat4 *view) {

  MeshUniform uModel = mesh_uniform_model(mesh);
  shader_update_uniform(mesh_shader_shadow(mesh), 0, view, sizeof(mat4), 0);
}

void material_shadow_set_cullmode(Mesh *mesh, WGPUCullMode mode) {
  /*
    STDPIPELINE SHADOW
  pipeline_set_primitive(shader_pipeline(mesh_shader_shadow(mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_Back,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

*/
}

void material_shadow_update_cullmode(Mesh *mesh, WGPUCullMode mode) {

  /*
  const Pipeline *pipeline = &mesh->shader.shadow.pipeline;
  wgpuRenderPipelineRelease(pipeline->handle);

  pipeline_set_primitive(pipeline,
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = mode,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  pipeline->handle = wgpuDeviceCreateRenderPipeline(pipeline->device,
&pipeline->descriptor); STDPIPELINE SHADOW
*/
}

/**
   Clear the shadow shader bind groups of mesh
 */
void material_shadow_clear_bindings(Mesh *mesh) {
  shader_bind_group_clear(mesh_shader_shadow(mesh));
}
