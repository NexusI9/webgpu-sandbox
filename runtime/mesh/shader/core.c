#include "core.h"
#include "../backend/renderer/scene/std_pipeline/std_pipeline.h"

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
Shader *mesh_shader_texture(Mesh *mesh) {
  return &mesh->shader.standard[MeshShader_Texture];
}

/**
   Return mesh shadow shader
 */
Shader *mesh_shader_shadow(Mesh *mesh) {
  return &mesh->shader.standard[MeshShader_Shadow];
}

/**
   Return mesh wireframe shader
 */
Shader *mesh_shader_wireframe(Mesh *mesh) {
  return &mesh->shader.standard[MeshShader_Wireframe];
}

/**
   Return mesh solid shader
 */
Shader *mesh_shader_solid(Mesh *mesh) {
  return &mesh->shader.standard[MeshShader_Solid];
}

/**
   Return mesh override shader
   Primarily used for fixed layer during the scene build/draw process.
 */
Shader *mesh_shader_fixed(Mesh *mesh) {
  return &mesh->shader.standard[MeshShader_Fixed];
}

Shader *mesh_shader_active(Mesh *mesh) { return mesh->shader.active; }

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
void mesh_shader_create_wireframe(Mesh *mesh) {

  Shader *wireframe_shader = mesh_shader_wireframe(mesh);

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
}

/**
   Initialize solid shader
 */
void mesh_shader_create_solid(Mesh *mesh) {

  Shader *solid_shader = mesh_shader_solid(mesh);

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
  // alias to shader_create
  shader_create(mesh_shader_texture(mesh), desc);
  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Texture);
}

/**
   Set texture shader.
 */
void mesh_shader_create_fixed(Mesh *mesh, const ShaderCreateDescriptor *desc) {
  // alias to shader_create
  shader_create(mesh_shader_fixed(mesh), desc);
  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Fixed);
}

/**
   Build Mesh, Camera and Projection matrix to a given mesh shader.
   It replaces the initial bound values by the ones provided by the scene
   (active camera matrix, viewport data).

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
void mesh_shader_build_mvp(Mesh *mesh, mesh_get_shader_callback target_shader,
                           Camera *camera, Viewport *viewport) {

  CameraUniform *uCamera = camera_uniform(camera);
  ViewportUniform *uViewport = viewport_uniform(viewport);
  MeshUniform *uMesh = mesh_uniform(mesh);

  Shader *shader = target_shader(mesh);

  // retrieve the model-view-projection binding index from the pipeline
  const PipelineBindingMVP *mvp = &shader->pipeline->bindings.mvp;

  ShaderBindGroupUniformEntry entries[3] = {
      // viewport
      {
          .binding = mvp->projection,
          .data = uViewport,
          .update = {0},
      },
      // camera
      {
          .binding = mvp->view,
          .data = uCamera,
          .update =
              {
                  .callback = camera_uniform_update_matrix_callback,
                  .trigger = camera_uniform_compare_views_callback,
                  .data = camera,
              },
      },
      // model
      {
          .binding = mvp->model,
          .data = uMesh,
          .update =
              {
                  .callback = mesh_uniform_model_update_callback,
                  .trigger = mesh_uniform_model_compare_callback,
                  .data = mesh,
              },
      },
  };

  for (size_t i = 0; i < 3; i++) {
    ShaderBindGroupUniformEntry *entry = &entries[i];
    shader_update_uniform(shader, mvp->group, entry->binding, entry->data);
    shader_update_uniform_callback(shader, mvp->group, entry->binding,
                                   &entry->update);
  }
}

/**

 */
void mesh_shader_update_mvp(Mesh *mesh, mesh_get_shader_callback target_shader,
                            Camera *camera, Viewport *viewport) {

  CameraUniform *uCamera = camera_uniform(camera);
  ViewportUniform *uViewport = viewport_uniform(viewport);
  MeshUniform *uMesh = mesh_uniform(mesh);

  Shader *shader = target_shader(mesh);

  // retrieve the model-view-projection binding index from the pipeline
  const PipelineBindingMVP *mvp = &shader->pipeline->bindings.mvp;

  ShaderBindGroupUniformEntry entries[3] = {
      // viewport
      {
          .binding = mvp->projection,
          .data = uViewport,
      },
      // camera
      {
          .binding = mvp->view,
          .data = uCamera,
      },
      // model
      {
          .binding = mvp->model,
          .data = uMesh,
      },
  };

  for (size_t i = 0; i < 3; i++) {
    ShaderBindGroupUniformEntry *entry = &entries[i];
    shader_update_uniform(shader, mvp->group, entry->binding, entry->data);
  }
}
