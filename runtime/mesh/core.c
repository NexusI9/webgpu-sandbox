#include "./core.h"
#include "../backend/buffer.h"
#include "../backend/shadow_pass.h"
#include "../light/light.h"
#include "../pipeline/pipeline.h"
#include "../runtime/geometry/edge/edge.h"
#include "../runtime/geometry/line/line.h"
#include "../utils/math.h"
#include "../utils/system.h"
#include "shader.h"
#include "topology/base.h"
#include "topology/core.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Shadow map is implicitely handled withing mesh
static Mesh *mesh_children_list_check_init(Mesh *);
static Mesh *mesh_children_list_check_capacity(Mesh *);

void mesh_create(Mesh *mesh, const MeshCreateDescriptor *md) {

  // init mesh

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
}

/**
   Create mesh from primitive index and vertex attributes
 */
void mesh_create_primitive(Mesh *mesh,
                           const MeshCreatePrimitiveDescriptor *md) {

  mesh_create(mesh, &(MeshCreateDescriptor){
                        .queue = md->queue,
                        .device = md->device,
                        .index = md->primitive.index,
                        .vertex = md->primitive.vertex,
                        .name = md->name,
                    });
}

void mesh_set_vertex_attribute(Mesh *mesh, const VertexAttribute *attributes) {

  MeshTopologyBase *base_vertex = &mesh->topology.base;

  // reset buffer
  if (base_vertex->attribute.buffer) {
    wgpuBufferRelease(base_vertex->attribute.buffer);
    base_vertex->attribute.buffer = NULL;
  }

  mesh->topology.base.attribute.entries = attributes->entries;
  mesh->topology.base.attribute.length = attributes->length;
  mesh->topology.base.attribute.capacity = attributes->length;

  if (base_vertex->attribute.length) {
    // store vertex in buffer
    mesh_create_vertex_buffer(
        mesh, &(MeshCreateBufferDescriptor){
                  .data = (void *)base_vertex->attribute.entries,
                  .size = base_vertex->attribute.length * sizeof(vattr_t),
              });

    // update wireframe shader as it requires mesh vertex & index
    // mesh_create_wireframe_shader(mesh);
  }
}

void mesh_set_vertex_index(Mesh *mesh, const VertexIndex *indexes) {

  MeshTopologyBase *base_vertex = &mesh->topology.base;
  // reset buffer
  if (base_vertex->index.buffer) {
    wgpuBufferRelease(base_vertex->index.buffer);
    base_vertex->index.buffer = NULL;
  }

  mesh->topology.base.index.entries = indexes->entries;
  mesh->topology.base.index.length = indexes->length;
  mesh->topology.base.index.capacity = indexes->length;

  if (base_vertex->index.length) {
    // store index in buffer
    mesh_create_index_buffer(
        mesh, &(MeshCreateBufferDescriptor){
                  .data = (void *)base_vertex->index.entries,
                  .size = base_vertex->attribute.length * sizeof(vindex_t),
              });

    // update wireframe shader as it requires mesh vertex & index
    // mesh_create_wireframe_shader(mesh);
  }
}

void mesh_set_parent(Mesh *child, Mesh *parent) { child->parent = parent; }

void mesh_set_name(Mesh *mesh, const char *name) {
  free(mesh->name);
  mesh->name = strdup(name);
}

void mesh_set_shader(Mesh *mesh, const ShaderCreateDescriptor *desc) {
  // alias to shader_create
  shader_create(&mesh->shader.texture, desc);
}

// send vertex data to GPU
void mesh_create_vertex_buffer(Mesh *mesh,
                               const MeshCreateBufferDescriptor *bd) {

  if (mesh->device == NULL || mesh->queue == NULL)
    perror("Mesh has no device or queue "), exit(0);

  buffer_create(&mesh->topology.base.attribute.buffer,
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
void mesh_create_index_buffer(Mesh *mesh,
                              const MeshCreateBufferDescriptor *bd) {

  if (mesh->device == NULL || mesh->queue == NULL)
    perror("Mesh has no device or queue"), exit(0);

  buffer_create(&mesh->topology.base.index.buffer,
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
   Build mesh shaders pipeline
   If given shader is NULL, it will choose the default shader as fallback
 */
void mesh_build(Mesh *mesh, Shader *shader) {

#ifdef VERBOSE_BUILDING_PHASE
  VERBOSE_PRINT("Build mesh: %s\n", mesh->name);
#endif

  // check if mesh has correct buffer before drawing
  if (mesh->topology.base.index.buffer == NULL ||
      mesh->topology.base.attribute.buffer == NULL) {
    perror("Mesh has no vertex index or attribute buffer.\n");
  }

  // build shader
  shader_build(shader);
}

/**
   Mesh main draw from default vertex and index buffer
 */
void mesh_draw(MeshTopology topology, Shader *shader,
               WGPURenderPassEncoder *render_pass, const Camera *camera,
               const Viewport *viewport) {

  // draw shader
  // if shader is null, use default shader
  shader_draw(shader, render_pass, camera, viewport);

  WGPUBuffer attribute_buffer = topology.attribute->buffer;
  WGPUBuffer index_buffer = topology.index->buffer;
  size_t index_length = topology.index->length;

  // draw indexes from buffer
  wgpuRenderPassEncoderSetVertexBuffer(*render_pass, 0, attribute_buffer, 0,
                                       WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderSetIndexBuffer(*render_pass, index_buffer,
                                      MESH_INDEX_FORMAT, 0, WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderDrawIndexed(*render_pass, index_length, 1, 0, 0, 0);
}

/**
   Apply scale to mesh transform matrix
 */
void mesh_scale(Mesh *mesh, vec3 scale) {
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
void mesh_translate(Mesh *mesh, vec3 position) {
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
void mesh_rotate(Mesh *mesh, vec3 rotation) {
  glm_vec3_copy(rotation, mesh->rotation);

  versor q;
  glm_euler_xyz_quat(mesh->rotation, q);
  mesh_rotate_quat(mesh, q);
}

/**
   Apply rotation to mesh transform matrix
 */
void mesh_rotate_quat(Mesh *mesh, versor rotation) {
  mat4 transform_matrix;
  mat4 dest;

  glm_quat_mat4(rotation, transform_matrix);
  glm_mat4_mul(mesh->model, transform_matrix, mesh->model);
}

/**
   Check if children list is already created.
   If not init a new list
 */
Mesh *mesh_children_list_check_init(Mesh *parent) {

  if (parent->children.entries == NULL) {
    parent->children.capacity = MESH_CHILD_LENGTH;
    parent->children.entries = calloc(parent->children.capacity, sizeof(Mesh));
  }

  return *parent->children.entries;
}

/**
   Check if children list has reached max capacity and reallocate or not
   accordingly
 */
Mesh *mesh_children_list_check_capacity(Mesh *parent) {

  if (parent->children.length == parent->children.capacity) {

    size_t new_capacity = parent->children.capacity * 2;
    Mesh *new_list = realloc(parent->children.entries,
                             sizeof(Mesh) * parent->children.capacity);

    if (new_list == NULL) {
      perror("Failed to expand mesh list\n"), exit(1);
      return NULL;
    }

    parent->children.entries = &new_list;
    parent->children.capacity = new_capacity;
  }

  return *parent->children.entries;
}

Mesh *mesh_new_child(Mesh *parent) {

  // init list
  mesh_children_list_check_init(parent);

  // expand parent mesh list
  mesh_children_list_check_capacity(parent);

  size_t id = parent->children.length;
  Mesh *child = parent->children.entries[id];

  // assing child id
  child->id = id;
  child->parent = parent; // assign parent to child

  // increment children length
  parent->children.length++;

  return child;
}

/**
   Add and initialize an empty child to the given mesh
 */
Mesh *mesh_new_child_empty(Mesh *mesh) {

  struct Mesh *temp_mesh = mesh_new_child(mesh);

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
Mesh *mesh_add_child(Mesh *child, Mesh *parent) {

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
Mesh *mesh_get_child(Mesh *mesh, size_t index) {
  return mesh->children.entries[index];
}

/**
   Return Mesh Base Vertex
 */
MeshTopology mesh_topology_base(Mesh *mesh) {
  return mesh_topology_base_vertex(&mesh->topology.base);
}

/**
   Return Mesh Wireframe Vertex
 */
MeshTopology mesh_topology_wireframe(Mesh *mesh) {
  return mesh_topology_wireframe_vertex(&mesh->topology.wireframe);
}
