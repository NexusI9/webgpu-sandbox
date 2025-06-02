#include "core.h"
#include "../backend/shadow_pass.h"


/**
   Bind Mesh, Camera and Projection matrix to a given mesh shader
   Note that the binding process follows a fixed convention of order, meaning
   one shall ensure the shader actually fits the bellow binding order:
   - Binding 0: Viewport projection matrix
   - Binding 1: Camera matrix
   - Binding 2: Model matrix
 */
void material_bind_views(Mesh *mesh, mesh_get_shader_callback target_shader,
                         Camera *camera, Viewport *viewport,
                         uint8_t group_index) {

  CameraUniform uCamera = camera_uniform(camera);
  ViewportUniform uViewport = viewport_uniform(viewport);
  MeshUniform uMesh = mesh_model_uniform(mesh);

  shader_add_uniform(
      target_shader(mesh),
      &(ShaderCreateUniformDescriptor){
          .group_index = group_index,
          .entry_count = 3,
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entries =
              (ShaderBindGroupUniformEntry[]){
                  // viewport
                  {
                      .binding = 0,
                      .data = &uViewport,
                      .size = sizeof(ViewportUniform),
                      .offset = 0,
                  },
                  // camera
                  {
                      .binding = 1,
                      .data = &uCamera,
                      .size = sizeof(CameraUniform),
                      .offset = 0,
                      .update_callback = camera_update_matrix_uniform,
                      .update_data = camera,
                  },
                  // model
                  {
                      .binding = 2,
                      .data = &uMesh,
                      .size = sizeof(MeshUniform),
                      .offset = 0,
                  },
              },
      });
}




