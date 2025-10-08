#include "./core.h"

#include <cglm/mat4.h>
#include <cglm/quat.h>
#include <cglm/vec3.h>
#include <stdlib.h>
#include <string.h>
#include <webgpu/webgpu.h>

#include "backend/logger.h"
#include "backend/registry.h"
#include "backend/ssbo.h"
#include "backend/std_pipeline/core.h"
#include "runtime/pipeline/render.h"
#include "shader/core.h"
#include "topology/boundbox.h"
#include "uniform.h"
#include "utils/dyli.h"

// Shadow map is implicitely handled withing mesh
static inline Mesh *mesh_children_list_check_init(Mesh *);

void mesh_create(Mesh *mesh, const MeshCreateDescriptor *md) {

  // set name
  mesh_set_name(mesh, md->name);

#ifdef VERBOSE_CREATING_PHASE
  logger_add(LoggerFlag_MeshCreate, "%s", mesh->name);
#endif

  mesh->id = reg_register((void *)mesh, RegEntryType_Mesh);

  // init child list
  mesh->children.length = 0;
  mesh->children.capacity = MESH_CHILD_LENGTH;
  mesh->children.entries = NULL;

  // set vertices & index for base topology
  if (md->vertex.length > 0 && md->index.length)
    mesh_topology_base_create(&mesh->topology.base, &md->vertex, &md->index);

  // init model matrix and transforms
  glm_mat4_identity(mesh->model);

  glm_quat_identity(mesh->rotation_quat);

  glm_vec3_copy(GLM_VEC3_ZERO, mesh->position);
  glm_vec3_copy(GLM_VEC3_ZERO, mesh->rotation_euler);
  glm_vec3_copy(GLM_VEC3_ONE, mesh->scale);

  // alloc uniform (may be replaced by SSBO later when added to the scene)
  ssbo_slot_init_alloc(&mesh->ssbo_slot, sizeof(MeshUniform));
  mesh_uniform_update(mesh);

  // set default pipeline shader
  mesh_shader(mesh, MeshShader_Texture)->pipeline =
      std_render_pipeline(RenderPipelineType_Default);

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
                        .index = md->primitive->index,
                        .vertex = md->primitive->vertex,
                        .name = md->name,
                    });
}

void mesh_set_parent(Mesh *child, Mesh *parent) { child->parent = parent; }

void mesh_set_name(Mesh *mesh, const char *name) {
  snprintf(mesh->name, MESH_NAME_LEN, "%s", name);
}

/**
   Mesh main draw from default vertex and index buffer
 */
void mesh_draw(MeshTopology topology, Shader *shader,
               WGPURenderPassEncoder render_pass) {

  // draw shader
  // if shader is null, use default shader
  shader_draw(shader, render_pass);

  WGPUBuffer attribute_buffer = topology.attribute->buffer;
  WGPUBuffer index_buffer = topology.index->buffer;
  size_t index_length = topology.index->length;

  // draw indexes from buffer
  wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, attribute_buffer, 0,
                                       WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderSetIndexBuffer(render_pass, index_buffer,
                                      MESH_INDEX_FORMAT, 0, WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderSetStencilReference(render_pass, 1);
  wgpuRenderPassEncoderDrawIndexed(render_pass, index_length, 1, 0, 0, 0);
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
    logger_add(LoggerFlag_Warning,
               "Couldn't create new entry in mesh child list.");
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
