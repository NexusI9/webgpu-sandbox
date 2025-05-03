#include "mesh.h"
#include "../backend/buffer.h"
#include "../backend/shadow_pass.h"
#include "../utils/system.h"
#include "light.h"
#include "pipeline.h"
#include "shader.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Shadow map is implicitely handled withing mesh
static void mesh_init_shadow_shader(mesh *);
static mesh *mesh_children_list_check_init(mesh *);
static mesh *mesh_children_list_check_capacity(mesh *);

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

  // init child list
  mesh->children.length = 0;
  mesh->children.capacity = MESH_CHILD_LENGTH;
  mesh->children.entries = NULL;

  // set wgpu
  mesh->device = md->device;
  mesh->queue = md->queue;

  // set vertices
  if (md->vertex.length > 0)
    mesh_set_vertex_attribute(mesh, &md->vertex);

  // set indexes
  if (md->index.length > 0)
    mesh_set_vertex_index(mesh, &md->index);

  // init model matrix and transforms
  glm_mat4_identity(mesh->model);
  glm_vec3_copy(GLM_VEC3_ZERO, mesh->position);
  glm_vec3_copy(GLM_VEC3_ZERO, mesh->rotation);
  glm_vec3_copy(GLM_VEC3_ONE, mesh->scale);

  // init shadow shader by default
  mesh_init_shadow_shader(mesh);
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

  // reset buffer
  if (mesh->buffer.vertex) {
    wgpuBufferRelease(mesh->buffer.vertex);
    mesh->buffer.vertex = NULL;
  }

  mesh->vertex.data = attributes->data;
  mesh->vertex.length = attributes->length;

  if (mesh->vertex.length) {
    // store vertex in buffer
    mesh_create_vertex_buffer(
        mesh, &(MeshCreateBufferDescriptor){
                  .data = (void *)mesh->vertex.data,
                  .size = mesh->vertex.length * sizeof(mesh->vertex.data[0]),
              });
  }
}

void mesh_set_vertex_index(mesh *mesh, const VertexIndex *indexes) {

  // reset buffer
  if (mesh->buffer.index) {
    wgpuBufferRelease(mesh->buffer.index);
    mesh->buffer.index = NULL;
  }

  mesh->index.data = indexes->data;
  mesh->index.length = indexes->length;

  if (mesh->index.length) {
    // store index in buffer
    mesh_create_index_buffer(
        mesh, &(MeshCreateBufferDescriptor){
                  .data = (void *)mesh->index.data,
                  .size = mesh->index.length * sizeof(mesh->index.data[0]),
              });
  }
}

void mesh_set_parent(mesh *child, mesh *parent) { child->parent = parent; }

void mesh_set_name(mesh *mesh, const char *name) {
  free(mesh->name);
  mesh->name = strdup(name);
}

void mesh_set_shader(mesh *mesh, const ShaderCreateDescriptor *desc) {
  // alias to shader_create
  shader_create(&mesh->shader.texture, desc);
}

// send vertex data to GPU
void mesh_create_vertex_buffer(mesh *mesh,
                               const MeshCreateBufferDescriptor *bd) {

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

/**
   Build mesh and children shaders pipeline
   If given shader is NULL, it will choose the default shader as fallback
 */
void mesh_build(mesh *mesh, MeshDrawMethod draw_method) {

  VERBOSE_PRINT("Build mesh: %s\n", mesh->name);
  shader *shader = mesh_select_shader(mesh, draw_method);

  // reccursively build shader
  shader_build(shader);

  // check if mesh has correct buffer before drawing
  if (mesh->buffer.index == NULL || mesh->buffer.vertex == NULL) {
    perror("Mesh doesn't have the right buffers, further error may occurs.\n");
  }

  // build children
  for (size_t c = 0; c < mesh->children.length; c++)
    mesh_build(mesh->children.entries[c], draw_method);
}

/**
   Mesh main draw function
 */
void mesh_draw(mesh *mesh, MeshDrawMethod draw_method,
               WGPURenderPassEncoder *render_pass, const camera *camera,
               const viewport *viewport) {

  // draw shader
  // if shader is null, use default shader
  shader *shader = mesh_select_shader(mesh, draw_method);
  shader_draw(shader, render_pass, camera, viewport);

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
  for (size_t c = 0; c < mesh->children.length; c++)
    mesh_draw(mesh->children.entries[c], draw_method, render_pass, camera,
              viewport);
}

/**
   Apply scale to mesh transform matrix
 */
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

/**
   Apply translation to mesh transform matrix
 */
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

/**
   Converts a vec3 rotation to quaternion and
   apply rotation to mesh transform matrix
 */
void mesh_rotate(mesh *mesh, vec3 rotation) {
  glm_vec3_copy(rotation, mesh->rotation);

  versor q;
  glm_euler_xyz(rotation, &q);
  mesh_rotate(mesh, q);
}

/**
   Apply rotation to mesh transform matrix
 */
void mesh_rotate_quat(mesh *mesh, versor rotation) {
  mat4 transform_matrix;
  glm_quat_mat4(rotation, transform_matrix);
  glm_mat4_mul(mesh->model, transform_matrix, mesh->model);
}

/**
   Check if children list is already created.
   If not init a new list
 */
mesh *mesh_children_list_check_init(mesh *parent) {

  if (parent->children.entries == NULL) {
    parent->children.capacity = MESH_CHILD_LENGTH;
    parent->children.entries = calloc(parent->children.capacity, sizeof(mesh));
    parent->children.index = calloc(parent->children.capacity, sizeof(size_t));
  }

  return *parent->children.entries;
}

/**
   Check if children list has reached max capacity and reallocate or not
   accordingly
 */
mesh *mesh_children_list_check_capacity(mesh *parent) {

  if (parent->children.length == parent->children.capacity) {

    size_t new_capacity = parent->children.capacity * 2;
    mesh *new_list = realloc(parent->children.entries,
                             sizeof(mesh) * parent->children.capacity);
    size_t *new_index = realloc(parent->children.index,
                                sizeof(size_t) * parent->children.capacity);

    if (new_list == NULL || new_index == NULL) {
      perror("Failed to expand mesh list\n"), exit(1);
      return NULL;
    }

    parent->children.entries = &new_list;
    parent->children.index = new_index;
    parent->children.capacity = new_capacity;
  }

  return *parent->children.entries;
}

mesh *mesh_new_child(mesh *parent) {

  // init list
  mesh_children_list_check_init(parent);

  // expand parent mesh list
  mesh_children_list_check_capacity(parent);

  size_t id = parent->children.length;
  mesh *child = parent->children.entries[id];

  // assing child id
  parent->children.index[id] = id;
  child->id = id;
  child->parent = parent; // assign parent to child

  // increment children length
  parent->children.length++;

  return child;
}

/**
   Add and initialize an empty child to the given mesh
 */
mesh *mesh_new_child_empty(mesh *mesh) {

  struct mesh *temp_mesh = mesh_new_child(mesh);

  // still need to initialize it before adding
  // this ensure proper init array
  mesh_create(temp_mesh, &(MeshCreateDescriptor){
                             .device = mesh->device,
                             .queue = mesh->queue,
                             .name = mesh->name,
                         });

  return temp_mesh;
}

/**
Add a new child pointer to the destination mesh children list
 */
mesh *mesh_add_child(mesh *child, mesh *parent) {

  // init list (?)
  mesh_children_list_check_init(parent);

  // expand parent mesh list (?)
  mesh_children_list_check_capacity(parent);

  // append pointer to the mesh list latest index
  parent->children.entries[parent->children.length++] = child;

  // assign parent pointer to child
  mesh_set_parent(child, parent);

  // return this same pointer
  return parent->children.entries[parent->children.length];
}

/**
   Retireve the mesh children address at the given index from the mesh children
   list
 */
mesh *mesh_get_child(mesh *mesh, size_t index) {
  return mesh->children.entries[index];
}

/**
   Return mesh default shader
 */
shader *mesh_shader_texture(mesh *mesh) { return &mesh->shader.texture; }

/**
   Return mesh shadow shader
 */
shader *mesh_shader_shadow(mesh *mesh) { return &mesh->shader.shadow; }

/**
   Init mesh shadow shader.
   By default all mesh have a shadow shader to generate shadow map
   during the bind light process we will generate the depth map since that's
   where we get out scene lights.

   The init shadow shadow doesn't belong to the material API as it is a
   necessary component set by default on mesh creation.
 */
void mesh_init_shadow_shader(mesh *mesh) {

  // import shadow shader
  shader *shadow_shader = mesh_shader_shadow(mesh);
  shader_create(shadow_shader,
                &(ShaderCreateDescriptor){
                    .path = "./runtime/assets/shader/shader.shadow.wgsl",
                    .label = "shadow",
                    .device = mesh->device,
                    .queue = mesh->queue,
                    .name = "shadow",
                });

  // edit shader pipeline (vertex only)
  pipeline_set_stencil(shader_pipeline(shadow_shader),
                       (WGPUDepthStencilState){
                           .format = SHADOW_DEPTH_FORMAT,
                           .depthWriteEnabled = true,
                           .depthCompare = WGPUCompareFunction_Less,
                       });

  /* need to set the cullback to FRONT for point light because the light POV
   * render is flipped on the X axis to match the cubemap coordinates, such
   * negative scaling lead to set the cullback to front.*/
  pipeline_set_primitive(shader_pipeline(shadow_shader),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_Front,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  for (size_t c = 0; c < mesh->children.length; c++)
    mesh_init_shadow_shader(mesh->children.entries[c]);
}

/**
Select the right mesh method call depending of the defined draw method
 */
shader *mesh_select_shader(mesh *mesh, MeshDrawMethod method) {

  switch (method) {

  case MESH_SHADER_SHADOW:
    return mesh_shader_shadow(mesh);
    break;

    // TODO: Implement other shader presets
  case MESH_SHADER_CUSTOM:
  case MESH_SHADER_SOLID:
  case MESH_SHADER_WIREFRAME:
  case MESH_SHADER_DEFAULT:
  default:
    return mesh_shader_texture(mesh);
    break;
  }
}
