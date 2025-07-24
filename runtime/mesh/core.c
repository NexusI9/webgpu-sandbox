#include "./core.h"
#include "../backend/buffer.h"
#include "../utils/matrix.h"
#include "../utils/system.h"
#include "shader.h"
#include "topology/boundbox.h"
#include <string.h>

// Shadow map is implicitely handled withing mesh
static Mesh *mesh_children_list_check_init(Mesh *);
static Mesh *mesh_children_list_check_capacity(Mesh *);
static void mesh_update_model_matrix(Mesh *);

void mesh_create(Mesh *mesh, const MeshCreateDescriptor *md) {

  // set name
  mesh_set_name(mesh, md->name);
  VERBOSE_MESH_CREATE("%s", mesh->name);

  mesh->id = reg_register((void *)mesh, RegEntryType_Mesh);

  // init child list
  mesh->children.length = 0;
  mesh->children.capacity = MESH_CHILD_LENGTH;
  mesh->children.entries = NULL;

  // set wgpu
  mesh->device = md->device;
  mesh->queue = md->queue;

  // set vertices & index for base topology
  if (md->vertex.length > 0 && md->index.length) {
    mesh_topology_base_create(&mesh->topology.base, &md->vertex, &md->index,
                              mesh->device, mesh->queue);
  }

  // init model matrix and transforms
  glm_mat4_identity(mesh->model);

  glm_quat_identity(mesh->rotation_quat);

  glm_vec3_copy(GLM_VEC3_ZERO, mesh->position);
  glm_vec3_copy(GLM_VEC3_ZERO, mesh->rotation_euler);
  glm_vec3_copy(GLM_VEC3_ONE, mesh->scale);

  // defines default override
  mesh_shader_set_override(mesh, mesh_shader_texture(mesh));
  mesh_topology_set_override(mesh,
                             (MeshTopology){
                                 .attribute = &mesh->topology.base.attribute,
                                 .index = &mesh->topology.base.index,
                             });
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

void mesh_set_parent(Mesh *child, Mesh *parent) { child->parent = parent; }

void mesh_set_name(Mesh *mesh, const char *name) {
  free(mesh->name);
  mesh->name = strdup(name);
}

void mesh_set_shader(Mesh *mesh, const ShaderCreateDescriptor *desc) {
  // alias to shader_create
  shader_create(&mesh->shader.texture, desc);
}

/**
   Build mesh shaders pipeline
   If given shader is NULL, it will choose the default shader as fallback
 */
void mesh_build(Mesh *mesh, Shader *shader) {

  // check if mesh has correct buffer before drawing
  if (mesh->topology.base.index.buffer == NULL ||
      mesh->topology.base.attribute.buffer == NULL)
    VERBOSE_ERROR("Mesh has no vertex index or attribute buffer.");

  // build shader
  shader_build(shader);
}

/**
   Mesh main draw from default vertex and index buffer
 */
void mesh_draw(MeshTopology topology, Shader *shader,
               WGPURenderPassEncoder *render_pass) {

  // draw shader
  // if shader is null, use default shader
  shader_draw(shader, render_pass);

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

  mesh_update_model_matrix(mesh);
}

/**
   Apply translation to mesh transform matrix
 */
void mesh_translate(Mesh *mesh, vec3 position) {
  glm_vec3_copy(position, mesh->position);

  mesh_update_model_matrix(mesh);
}

/**
   Set Euler rotation
 */
void mesh_rotate(Mesh *mesh, vec3 rotation) {
  // cache euler rotation
  glm_vec3_copy(rotation, mesh->rotation_euler);

  // update quat from euler
  vec3 rad_rotation;
  glm_vec3_scale(mesh->rotation_euler, GLM_PI / 180.0f, rad_rotation);

  glm_euler_xyz_quat(rad_rotation, mesh->rotation_quat);

  // recompute model matrix
  mesh_update_model_matrix(mesh);
}

/**
   Apply rotation to mesh transform matrix
 */
void mesh_rotate_quat(Mesh *mesh, versor rotation) {

  // cache quat rotation
  glm_quat_copy(rotation, mesh->rotation_quat);

  // update mesh euler rotation
  mat4 rot;
  glm_quat_mat4(mesh->rotation_quat, rot);

  vec3 rad_euler;
  glm_euler_angles(rot, rad_euler);
  glm_vec3_scale(rad_euler, 180.0f / GLM_PI, mesh->rotation_euler);

  // recompute model matrix
  mesh_update_model_matrix(mesh);
}

/**
   Apply look at transformation to mesh
 */
void mesh_lookat(Mesh *mesh, vec3 position, vec3 target) {

  glm_mat4_identity(mesh->model);

  matrix_mesh_lookat(&(UtilsMatrixLookatDescriptor){
      .dest_position = &mesh->position,
      .dest_matrix = &mesh->model,
      .position = position,
      .target = target,
      .up = NULL,
      .forward = NULL,
      .right = NULL,
  });
}

/**
   Update mesh model matrix on the right order based on the cached position,
   rotation and scale.
   This function is called everytime the mesh follows a spatial transformation.
 */
void mesh_update_model_matrix(Mesh *mesh) {

  mat4 S, R, T, SR;

  glm_mat4_identity(S);
  glm_scale(S, mesh->scale);

  glm_quat_mat4(mesh->rotation_quat, R);

  glm_mat4_identity(T);
  glm_translate(T, mesh->position);

  glm_mat4_mul(R, S, SR);
  glm_mat4_mul(T, SR, mesh->model);

  // update topologies
  mesh_topology_boundbox_update(&mesh->topology.base, mesh->model,
                                &mesh->topology.boundbox, mesh->queue);
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
      VERBOSE_ERROR("Failed to expand mesh list.");
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

/**
   Return Mesh Boundbox Vertex
 */
MeshTopology mesh_topology_boundbox(Mesh *mesh) {
  return mesh_topology_boundbox_vertex(&mesh->topology.boundbox);
}

/**
   Override topology is primarily used for fixed mesh during the scene build and
   draw phase and will be the targeted topology for whatever render mode
   (solid/wireframe/texture)
 */
MeshTopology mesh_topology_override(Mesh *mesh) {
  return mesh->topology.override;
}

/**
   Define the override topology.
 */
void mesh_topology_set_override(Mesh *mesh, const MeshTopology topology) {
  mesh->topology.override = topology;
}
