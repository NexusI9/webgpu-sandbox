#include "./core.h"
#include "../backend/buffer.h"
#include "../backend/renderer/scene/std_pipeline/std_pipeline.h"
#include "../utils/dyli.h"
#include "../utils/matrix.h"
#include "shader/core.h"
#include "shader/shader.h"
#include "shader/texture.h"
#include "topology/boundbox.h"
#include <string.h>

#include "../utils/system.h"
#include "uniform.h"

// Shadow map is implicitely handled withing mesh
static inline Mesh *mesh_children_list_check_init(Mesh *);

void mesh_create(Mesh *mesh, const MeshCreateDescriptor *md) {

  // set name
  mesh_set_name(mesh, md->name);

#ifdef VERBOSE_CREATING_PHASE
  VERBOSE_MESH_CREATE("%s", mesh->name);
#endif

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

  // create uniform
  mesh_uniform_update(mesh);

  // set default pipeline shader
  mesh_shader_texture(mesh)->pipeline = std_pipeline(PipelineType_Default);

  // defines default topology override
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
                        .index = md->primitive->index,
                        .vertex = md->primitive->vertex,
                        .name = md->name,
                    });
}

void mesh_set_parent(Mesh *child, Mesh *parent) { child->parent = parent; }

void mesh_set_name(Mesh *mesh, const char *name) {
  if (mesh->name)
    free(mesh->name);
  mesh->name = strdup(name);
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
   Check if children list is already created.
   If not init a new list
 */
Mesh *mesh_children_list_check_init(Mesh *parent) {

  if (parent->children.entries == NULL)
    dyli_create((void *)&parent->children.entries, &parent->children.capacity,
                &parent->children.length, sizeof(Mesh *), 16,
                "Mesh children list");

  return *parent->children.entries;
}

Mesh *mesh_child_new(Mesh *parent) {

  // init list
  mesh_children_list_check_init(parent);

  Mesh *child = dyli_new_entry(
      (void *)&parent->children.entries, &parent->children.capacity,
      &parent->children.length, sizeof(Mesh *), "Mesh child list");

  if (child == NULL) {
    VERBOSE_WARNING("Couldn't create new entry in mesh child list.");
    return NULL;
  }

  child->parent = parent;

  return child;
}

/**
Add a new child pointer to the destination mesh children list
 */
DynamicListStatus mesh_child_add(Mesh *parent, Mesh *child) {

  mesh_children_list_check_init(parent);

  DynamicListStatus insert =
      dyli_insert((void *)&parent->children.entries, &parent->children.capacity,
                  &parent->children.length, sizeof(Mesh *), (void *)&child, 1,
                  "Mesh children list");

  if (insert == DynamicListStatus_Success) {
    // assign parent pointer to child
    child->parent = parent;
  }

  return insert;
}

/**
   Retireve the mesh children address at the given index from the mesh children
   list
 */
Mesh *mesh_child_get(Mesh *mesh, size_t index) {
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

void mesh_get_position(Mesh *mesh, vec3 *dest) {
  glm_vec3_copy(mesh->position, *dest);
}

void mesh_get_scale(Mesh *mesh, vec3 *dest) {
  glm_vec3_copy(mesh->scale, *dest);
}

void mesh_get_rotation_euler(Mesh *mesh, vec3 *dest) {
  glm_vec3_copy(mesh->rotation_euler, *dest);
}
