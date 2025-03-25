#include "mesh.h"
#include "../backend/buffer.h"
#include "../include/cglm/euler.h"
#include "../include/cglm/quat.h"
#include "../utils/system.h"
#include "light.h"
#include "shader.h"
#include "webgpu/webgpu.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

void mesh_create(mesh *mesh, const MeshCreateDescriptor *md) {

  // set name
  mesh_set_name(mesh, md->name);

  // set wgpu
  mesh->device = md->device;
  mesh->queue = md->queue;

  // set vertices
  if (md->vertex.length > 0)
    mesh_set_vertex_attribute(mesh, &md->vertex);

  // set indexes
  if (md->index.length > 0)
    mesh_set_vertex_index(mesh, &md->index);

  // TODO: uniformise shader creation (ref || value)
  mesh->shader = md->shader;

  // init model matrix
  glm_mat4_identity(mesh->model);

  // init child list
  mesh->children.length = 0;
  mesh->children.capacity = 0;
  mesh->children.items = NULL;
}

void mesh_create_primitive(mesh *mesh,
                           const MeshCreatePrimitiveDescriptor *md) {
  mesh_create(mesh, &(MeshCreateDescriptor){
                        .queue = md->queue,
                        .device = md->device,
                        .index = md->primitive.index,
                        .vertex = md->primitive.vertex,
                        .name = md->name,
                    });
}

void mesh_set_vertex_attribute(mesh *mesh, const VertexAttribute *attributes) {

  mesh->vertex.data = attributes->data;
  mesh->vertex.length = attributes->length;

  // store vertex in buffer
  mesh_create_vertex_buffer(
      mesh, &(MeshCreateBufferDescriptor){
                .data = (void *)mesh->vertex.data,
                .size = mesh->vertex.length * sizeof(mesh->vertex.data[0]),
            });
}

void mesh_set_vertex_index(mesh *mesh, const VertexIndex *indexes) {

  mesh->index.data = indexes->data;
  mesh->index.length = indexes->length;

  // store index in buffer
  mesh_create_index_buffer(
      mesh, &(MeshCreateBufferDescriptor){
                .data = (void *)mesh->index.data,
                .size = mesh->index.length * sizeof(mesh->index.data[0]),
            });
}

void mesh_set_parent(mesh *child, mesh *parent) { child->parent = parent; }

void mesh_set_name(mesh *mesh, const char *name) {
  free(mesh->name);
  mesh->name = strdup(name);
}

void mesh_set_shader(mesh *mesh, const ShaderCreateDescriptor *desc) {
  // alias to shader_create
  shader_create(&mesh->shader, desc);
}

// send vertex data to GPU
void mesh_create_vertex_buffer(mesh *mesh,
                               const MeshCreateBufferDescriptor *bd) {

  /*printf("name:%s, device: %p, queue:%p\n", mesh->name, mesh->device,
    mesh->queue);*/
  if (mesh->device == NULL || mesh->queue == NULL)
    perror("Mesh has no device or queue "), exit(0);

  buffer_create(&mesh->buffer.vertex,
                &(CreateBufferDescriptor){
                    .queue = mesh->queue,
                    .device = mesh->device,
                    .data = (void *)bd->data,
                    .size = bd->size,
                    .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });
}

// send index data to GPU
void mesh_create_index_buffer(mesh *mesh,
                              const MeshCreateBufferDescriptor *bd) {

  if (mesh->device == NULL || mesh->queue == NULL)
    perror("Mesh has no device or queue"), exit(0);

  buffer_create(&mesh->buffer.index,
                &(CreateBufferDescriptor){
                    .queue = mesh->queue,
                    .device = mesh->device,
                    .data = (void *)bd->data,
                    .size = bd->size,
                    .usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });
}

void mesh_build(mesh *mesh) {

  // reccursively build shader
  shader_build(&mesh->shader);

  // build children
  if (mesh->children.items != NULL) {
    for (size_t c = 0; c < mesh->children.length; c++) {
      mesh_build(&mesh->children.items[c]);
    }
  }
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

  // draw children
  // TODO: REDUCE DRAW CALL.........
  if (mesh->children.items != NULL) {
    for (size_t c = 0; c < mesh->children.length; c++) {
      mesh_draw(&mesh->children.items[c], render_pass, camera, viewport);
    }
  }
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
                                        .entries = entries,
                                    });

  if (mesh->children.items != NULL) {
    for (size_t c = 0; c < mesh->children.length; c++) {
      mesh_bind_matrices(&mesh->children.items[c], camera, viewport,
                         group_index);
    }
  }
}

void mesh_bind_lights(mesh *mesh, AmbientLightList *ambient_list,
                      DirectionalLightList *directional_list,
                      PointLightList *point_list, uint8_t group_index) {

  // init light list uniforms
  // due to WGSL array uniforms necessity to have constant size, uniforms lists
  // are already set at <light_list>[12], init them all to 0
  // by default we will upload all the lights (point, ambient, directional)
  // within a defined group
  AmbientLightListUniform ambient_uniform = {0};
  DirectionalLightListUniform directional_uniform = {0};
  PointLightListUniform point_uniform = {0};

  if (ambient_list) {
    // update length
    ambient_uniform.length = ambient_list->length;
    // update entries
    for (size_t i = 0; i < ambient_uniform.length; i++)

      ambient_uniform.items[i] = ambient_list->items[i];
  }

  if (directional_list) {
    // update length
    directional_uniform.length = directional_list->length;
    // update entries
    for (size_t i = 0; i < directional_uniform.length; i++)
      directional_uniform.items[i] = directional_list->items[i];
  }

  if (point_list) {
    // update length
    point_uniform.length = point_list->length;
    // update entries
    for (size_t i = 0; i < point_uniform.length; i++)
      point_uniform.items[i] = point_list->items[i];
  }

  ShaderBindGroupEntry entries[3] = {
      // ambient light
      {
          .binding = 0,
          .data = &ambient_uniform,
          .offset = 0,
          .size = sizeof(AmbientLightListUniform),
      },
      // directional light
      {
          .binding = 1,
          .data = &directional_uniform,
          .offset = 0,
          .size = sizeof(DirectionalLightListUniform),
      },
      // point light
      {
          .binding = 2,
          .data = &point_uniform,
          .offset = 0,
          .size = sizeof(PointLightListUniform),
      },
  };

  shader_add_uniform(&mesh->shader, &(ShaderCreateUniformDescriptor){
                                        .group_index = group_index,
                                        .entry_count = 3,
                                        .visibility = WGPUShaderStage_Vertex |
                                                      WGPUShaderStage_Fragment,
                                        .entries = entries,
                                    });
}

size_t mesh_add_child(mesh *child, mesh *dest) {

  // init list
  if (dest->children.items == NULL) {
    dest->children.capacity = MESH_CHILD_LENGTH;
    dest->children.items = malloc(sizeof(mesh) * dest->children.capacity);
    dest->children.index = malloc(sizeof(size_t) * dest->children.capacity);
  }

  // expand list
  if (dest->children.length == dest->children.capacity) {
    size_t new_capacity = dest->children.capacity * 2;
    mesh *new_list =
        realloc(dest->children.items, sizeof(mesh) * dest->children.capacity);
    size_t *new_index =
        realloc(dest->children.items, sizeof(size_t) * dest->children.capacity);

    if (new_list == NULL || new_index == NULL) {
      perror("Failed to expand mesh list\n"), exit(1);
    }

    dest->children.items = new_list;
    dest->children.index = new_index;
    dest->children.capacity = new_capacity;
  }

  size_t id = dest->children.length;

  // assign child to parent
  dest->children.items[id] = *child;

  // assing child id
  dest->children.index[id] = id;
  dest->children.items[id].id = id;

  // assign parent to child
  dest->children.items[id].parent = dest;
  dest->children.length++;

  return id;
}

size_t mesh_add_child_empty(mesh *mesh) {

  // add empty mesh, still need to initialize it before adding
  // this ensure proper init array
  struct mesh temp_mesh;

  mesh_create(&temp_mesh, &(MeshCreateDescriptor){
                              .device = mesh->device,
                              .queue = mesh->queue,
                          });

  return mesh_add_child(&temp_mesh, mesh);
}

mesh *mesh_get_child(mesh *mesh, size_t index) {
  return &mesh->children.items[index];
}
