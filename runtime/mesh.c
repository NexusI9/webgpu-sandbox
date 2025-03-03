#include "mesh.h"
#include "../backend/generator.h"
#include "../include/cglm/euler.h"
#include "../include/cglm/quat.h"
#include "../utils/system.h"
#include "shader.h"
#include "webgpu/webgpu.h"
#include <stdio.h>

MeshUniform mesh_model_uniform(mesh *mesh) {

  MeshUniform uModel;

  vec4 position = {
      mesh->position[0],
      mesh->position[1],
      mesh->position[2],
      1.0f,
  };

  glm_mat4_copy(mesh->model, uModel.model);
  glm_vec4_copy(position, uModel.position);

  return uModel;
}

mesh mesh_create(const MeshCreateDescriptor *md) {

  mesh new_mesh;

  // set wgpu
  new_mesh.wgpu.device = md->wgpu.device;
  new_mesh.wgpu.queue = md->wgpu.queue;

  // set vertices
  new_mesh.vertex.data = md->vertex.data;
  new_mesh.vertex.length = md->vertex.length;

  // set indexes
  new_mesh.index.data = md->index.data;
  new_mesh.index.length = md->index.length;

  // set shader
  new_mesh.shader = md->shader;

  // init model matrix
  glm_mat4_identity(new_mesh.model);

  // init child list
  new_mesh.children.length = 0;
  new_mesh.children.capacity = 0;
  new_mesh.children.items = NULL;

  // store vertex in buffer
  mesh_create_vertex_buffer(
      &new_mesh,
      &(MeshCreateBufferDescriptor){
          .data = (void *)new_mesh.vertex.data,
          .size = new_mesh.vertex.length * sizeof(new_mesh.vertex.data[0]),
      });

  // store index in buffer
  mesh_create_index_buffer(
      &new_mesh,
      &(MeshCreateBufferDescriptor){
          .data = (void *)new_mesh.index.data,
          .size = new_mesh.index.length * sizeof(new_mesh.vertex.data[0]),
      });

  return new_mesh;
}

mesh mesh_create_primitive(const MeshCreatePrimitiveDescriptor *md) {
  return mesh_create(&(MeshCreateDescriptor){
      .wgpu = md->wgpu,
      .index = md->primitive.index,
      .vertex = md->primitive.vertex,
      .shader = md->shader,
  });
}

// send vertex data to GPU
void mesh_create_vertex_buffer(mesh *mesh,
                               const MeshCreateBufferDescriptor *bd) {

  if (mesh->wgpu.device == NULL || mesh->wgpu.queue == NULL)
    perror("Mesh has no device or queue"), exit(0);

  mesh->buffer.vertex = create_buffer(&(CreateBufferDescriptor){
      .queue = mesh->wgpu.queue,
      .device = mesh->wgpu.device,
      .data = (void *)bd->data,
      .size = bd->size,
      .usage = WGPUBufferUsage_Vertex,
  });
}

// send index data to GPU
void mesh_create_index_buffer(mesh *mesh,
                              const MeshCreateBufferDescriptor *bd) {

  if (mesh->wgpu.device == NULL || mesh->wgpu.queue == NULL)
    perror("Mesh has no device or queue"), exit(0);

  mesh->buffer.index = create_buffer(&(CreateBufferDescriptor){
      .queue = mesh->wgpu.queue,
      .device = mesh->wgpu.device,
      .data = (void *)bd->data,
      .size = bd->size,
      .usage = WGPUBufferUsage_Index,
  });
}

void mesh_draw(mesh *mesh, WGPURenderPassEncoder *render_pass,
               const camera *camera, const viewport *viewport) {

  // draw shader
  shader_draw(&mesh->shader, render_pass, camera, viewport);

  // draw indexes from buffer
  wgpuRenderPassEncoderSetVertexBuffer(*render_pass, 0, mesh->buffer.vertex, 0,
                                       WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderSetIndexBuffer(*render_pass, mesh->buffer.index,
                                      WGPUIndexFormat_Uint16, 0,
                                      WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderDrawIndexed(*render_pass, mesh->index.length, 1, 0, 0,
                                   0);
}

void mesh_scale(mesh *mesh, vec3 scale) {
  glm_vec3_copy(scale, mesh->scale);

  mat4 transform_matrix = {
      {scale[0], 0.0f, 0.0f, 0.0f},
      {0.0f, scale[1], 0.0f, 0.0f},
      {0.0f, 0.0f, scale[2], 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
  };

  glm_mat4_mul(mesh->model, transform_matrix, mesh->model);
}
void mesh_position(mesh *mesh, vec3 position) {
  glm_vec3_copy(position, mesh->position);

  mat4 transform_matrix = {
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {position[0], position[1], position[2], 1.0f},
  };

  glm_mat4_mul(mesh->model, transform_matrix, mesh->model);
}

void mesh_rotate(mesh *mesh, vec3 rotation) {
  glm_vec3_copy(rotation, mesh->rotation);

  versor q;
  glm_euler_xyz(rotation, &q);

  mat4 transform_matrix;
  glm_quat_mat4(q, transform_matrix);

  glm_mat4_mul(mesh->model, transform_matrix, mesh->model);
}

void mesh_bind_matrices(mesh *mesh, camera *camera, viewport *viewport,
                        uint8_t group_index) {

  ShaderViewProjectionUniform proj_view_data;

  CameraUniform uCamera = camera_uniform(camera);
  ViewportUniform uViewport = viewport_uniform(viewport);
  MeshUniform uMesh = mesh_model_uniform(mesh);

  ShaderBindGroupEntry entries[3] = {
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
  };

  shader_add_uniform(&mesh->shader, &(ShaderCreateUniformDescriptor){
                                        .group_index = group_index,
                                        .entry_count = 3,
                                        .visibility = WGPUShaderStage_Vertex |
                                                      WGPUShaderStage_Fragment,
                                        .entries = entries});
}

mesh *mesh_add_child(mesh child, mesh *dest) {

  // init list
  if (dest->children.items == NULL) {
    dest->children.capacity = MESH_CHILD_LENGTH;
    dest->children.items = malloc(sizeof(mesh) * dest->children.capacity);
  }

  // expand list
  if (dest->children.length == dest->children.capacity) {
    size_t new_capacity = dest->children.capacity * 2;
    mesh *new_list =
        realloc(dest->children.items, sizeof(mesh) * dest->children.capacity);
    if (new_list == NULL) {
      perror("Failed to expand mesh list\n"), exit(1);
      return NULL;
    }

    dest->children.items = new_list;
    dest->children.capacity = new_capacity;
  }

  dest->children.items[dest->children.length++] = child;

  return &dest->children.items[dest->children.length - 1];
}
