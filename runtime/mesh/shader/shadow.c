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

void mesh_shader_shadow_update_mvp(Mesh *mesh) {

  MeshUniform* uModel = mesh_uniform(mesh);
  
  // views uniforms (will be replaced during shadow pass)
  shader_update_uniform(mesh_shader_shadow(mesh), 0, 0, (void *)0);

  // mesh model matrix
  shader_update_uniform(mesh_shader_shadow(mesh), 0, 1, uModel);
}

void mesh_shader_shadow_update_view(Mesh *mesh, mat4 *view) {
  shader_update_uniform(mesh_shader_shadow(mesh), 0, 0, view);
}

void mesh_shader_shadow_set_cullmode(Mesh *mesh, WGPUCullMode mode) {
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

void mesh_shader_shadow_update_cullmode(Mesh *mesh, WGPUCullMode mode) {

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
void mesh_shader_shadow_clear_bindings(Mesh *mesh) {
  shader_bind_group_clear(mesh_shader_shadow(mesh));
}
