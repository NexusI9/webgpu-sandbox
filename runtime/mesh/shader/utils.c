#include "./utils.h"

#include "../utils/system.h"
/**
   Bind Mesh, Camera and Projection matrix to a given mesh shader
   Note that the binding process follows a fixed convention of order, meaning
   one shall ensure the shader actually fits the bellow binding order:
   - Binding 0: Viewport projection matrix
   - Binding 1: Camera matrix
   - Binding 2: Model matrix
 */
void mesh_shader_bind_views(Mesh *mesh, mesh_get_shader_callback target_shader,
                         Camera *camera, Viewport *viewport) {

  CameraUniform uCamera = camera_uniform(camera);
  ViewportUniform uViewport = viewport_uniform(viewport);
  MeshUniform uMesh = mesh_uniform_model(mesh);

  Shader *shader = target_shader(mesh);

  // retrieve the model-view-projection binding index from the pipeline
  const PipelineBindingMVP *mvp = &shader->pipeline->bindings.mvp;

  shader_add_uniform(
      shader,
      &(ShaderCreateUniformDescriptor){
          .group_index = mvp->group,
          .entry_count = 3,
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
          .entries =
              (ShaderBindGroupUniformEntry[]){
                  // viewport
                  {
                      .binding = mvp->projection,
                      .data = &uViewport,
                      .size = sizeof(ViewportUniform),
                      .offset = 0,
                  },
                  // camera
                  {
                      .binding = mvp->view,
                      .data = &uCamera,
                      .size = sizeof(CameraUniform),
                      .offset = 0,
                      /* .update =
                          {
                              .callback = camera_uniform_update_matrix,
                              .trigger = camera_uniform_compare_views,
                              .data = camera,
                          },*/
                  },
                  // model
                  {
                      .binding = mvp->model,
                      .data = &uMesh,
                      .size = sizeof(MeshUniform),
                      .offset = 0,
                      /*.update =
                          {
                              .callback = mesh_uniform_model_update,
                              .trigger = mesh_uniform_model_compare,
                              .data = mesh,
                          },*/
                  },
              },
      });
}
