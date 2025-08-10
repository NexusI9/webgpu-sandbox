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
void mesh_shader_bind_views_any(Mesh *mesh,
                                mesh_get_shader_callback target_shader,
                                Camera *camera, Viewport *viewport) {

  CameraUniform uCamera = camera_uniform(camera);
  ViewportUniform uViewport = viewport_uniform(viewport);
  MeshUniform uMesh = mesh_uniform_model(mesh);

  Shader *shader = target_shader(mesh);

  // retrieve the model-view-projection binding index from the pipeline
  const PipelineBindingMVP *mvp = &shader->pipeline->bindings.mvp;

  ShaderBindGroupUniformEntry entries[3] = {
      // viewport
      {
          .binding = mvp->projection,
          .data = &uViewport,
      },
      // camera
      {
          .binding = mvp->view,
          .data = &uCamera,
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
          /*.update =
              {
                  .callback = mesh_uniform_model_update,
                  .trigger = mesh_uniform_model_compare,
                  .data = mesh,
              },*/
      },
  };

  for (size_t i = 0; i < 3; i++) {
    ShaderBindGroupUniformEntry *entry = &entries[i];
    shader_update_uniform(shader, mvp->group, entry->binding, entry->data);
  }
}
