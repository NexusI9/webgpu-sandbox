#include "mesh.h"
#include "../backend/buffer.h"
#include "../backend/shadow_pass.h"
#include "../resources/debug/line.h"
#include "../resources/geometry/edge.h"
#include "../utils/math.h"
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
static void mesh_init_wireframe_shader(mesh *);
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

  // init wireframe shader by default
  // TODO: maybe only enable wireframe on "edit" mode and not on "production"
  mesh_init_wireframe_shader(mesh);
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
  mesh->vertex.capacity = attributes->length;

  if (mesh->vertex.length) {
    // store vertex in buffer
    mesh_create_vertex_buffer(
        mesh, &(MeshCreateBufferDescriptor){
                  .data = (void *)mesh->vertex.data,
                  .size = mesh->vertex.length * sizeof(mesh->vertex.data[0]),
              });

    // update wireframe shader as it requires mesh vertex & index
    mesh_init_wireframe_shader(mesh);
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
  mesh->index.capacity = indexes->length;

  if (mesh->index.length) {
    // store index in buffer
    mesh_create_index_buffer(
        mesh, &(MeshCreateBufferDescriptor){
                  .data = (void *)mesh->index.data,
                  .size = mesh->index.length * sizeof(mesh->index.data[0]),
              });

    // update wireframe shader as it requires mesh vertex & index
    mesh_init_wireframe_shader(mesh);
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
   Build mesh shaders pipeline
   If given shader is NULL, it will choose the default shader as fallback
 */
void mesh_build(mesh *mesh, MeshDrawMethod draw_method) {

#ifdef VERBOSE_BUILDING_PHASE
  VERBOSE_PRINT("Build mesh: %s\n", mesh->name);
#endif

  shader *shader = mesh_select_shader(mesh, draw_method);

  // reccursively build shader
  shader_build(shader);

  // check if mesh has correct buffer before drawing
  if (mesh->buffer.index == NULL || mesh->buffer.vertex == NULL) {
    perror("Mesh has no device or queue, further error may occurs.\n");
  }
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
   Return mesh wireframe shader
 */
shader *mesh_shader_wireframe(mesh *mesh) { return &mesh->shader.wireframe; }

/**
   Init mesh shadow shader.
   By default all mesh have a shadow shader to generate shadow map
   during the bind light process we will generate the depth map since that's
   where we get out scene lights.

   The init shadow shader doesn't belong to the material API as it is a
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
void mesh_init_wireframe_shader(mesh *mesh) {

  WGPUBuffer vertex_buffer = mesh->buffer.wireframe.vertex;
  WGPUBuffer index_buffer = mesh->buffer.wireframe.index;

  // reset existing wireframe buffer if exists
  if (vertex_buffer) {
    wgpuBufferRelease(vertex_buffer);
    vertex_buffer = NULL;
  }

  if (index_buffer) {
    wgpuBufferRelease(index_buffer);
    index_buffer = NULL;
  }

  /*
    Create a edge hash set to store unique edges
    To store unique data, hash tables are more efficient since we can directly
    check if the hash exist via the hashing function instead of every time
    traverse the array and compate the data.

      Object                                  Hash Table
    .--------.                              .-------------.
    | attr a | --.   .-------------.        |     ..      |
    |--------|   '-- |   Hashing   | --.    |-------------|
    | attr b | --'   '-------------'   |    |     ..      |
    '--------'                         |    |-------------|
                                       '--> |     34      |
                                            |-------------|
                                            |     ..      |
                                            '-------------'

               Data Attributes + Hash = Index

   */

  EdgeHashSet edges;
  edge_hash_set_create(&edges, 40);

  for (int i = 0; i < mesh->index.length; i += 3) {
    unsigned int a = mesh->index.data[i];
    unsigned int b = mesh->index.data[i + 1];
    unsigned int c = mesh->index.data[i + 2];

    EdgeKey ab = {MIN(a, b), MAX(a, b)};
    EdgeKey bc = {MIN(b, c), MAX(b, c)};
    EdgeKey ca = {MIN(a, c), MAX(a, c)};

    edge_hash_set_insert(&edges, ab);
    edge_hash_set_insert(&edges, bc);
    edge_hash_set_insert(&edges, ca);
  }

  // temp arrays from edges
  size_t vertex_capacity = edges.length * LINE_VERTEX_COUNT * VERTEX_STRIDE;
  float wireframe_vertex_attribute[vertex_capacity];
  VertexAttribute temp_vertex_attribute = {
      .data = wireframe_vertex_attribute,
      .capacity = vertex_capacity,
      .length = 0,
  };

  size_t index_capacity = edges.length * LINE_INDEX_COUNT;
  uint16_t wireframe_index_attribute[index_capacity];
  VertexIndex temp_vertex_index = {
      .data = wireframe_index_attribute,
      .capacity = index_capacity,
      .length = 0,
  };

  // go through unique edges set add populate temp vertex & index array
  for (int l = 0; l < edges.length; l++) {

    size_t index = edges.occupied[l];
    EdgeBucket *current_edge = &edges.entries[index];

    // base vertex
    int base_index = current_edge->key[0];
    float *base_attributes = &mesh->vertex.data[base_index * VERTEX_STRIDE];
    vertex base_vertex = vertex_from_array(base_attributes);

    // opposite vertex
    int opp_index = current_edge->key[1];
    float *opp_attributes = &mesh->vertex.data[opp_index * VERTEX_STRIDE];
    vertex opp_vertex = vertex_from_array(opp_attributes);

    // TODO: make dynamic wireframe color
    vec3 color = {0.0f, 1.0f, 0.0f};

    // add points to vertex attributes and index
    line_add_point(base_vertex.position, opp_vertex.position, color,
                   &temp_vertex_attribute, &temp_vertex_index);
  }

  // upload vertex attribute and index to wireframe buffer

  // upload vertex attributes
  buffer_create(&mesh->buffer.wireframe.vertex,
                &(CreateBufferDescriptor){
                    .queue = mesh->queue,
                    .device = mesh->device,
                    .data = (void *)temp_vertex_attribute.data,
                    .size = temp_vertex_attribute.length * sizeof(float),
                    .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });

  // upload vertex index
  buffer_create(&mesh->buffer.wireframe.index,
                &(CreateBufferDescriptor){
                    .queue = mesh->queue,
                    .device = mesh->device,
                    .data = (void *)temp_vertex_index.data,
                    .size = temp_vertex_index.length * sizeof(uint16_t),
                    .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });
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
  case MESH_SHADER_WIREFRAME:
    return mesh_shader_wireframe(mesh);
    break;

  case MESH_SHADER_CUSTOM:
  case MESH_SHADER_SOLID:
  case MESH_SHADER_DEFAULT:
  default:
    return mesh_shader_texture(mesh);
    break;
  }
}
