#include "transform.h"
#include "../material/material.h"
#include "../resources/loader/loader.mbin.h"

void gizmo_transform_create_mesh(
    MeshRefList *list, const GizmoTransformCreateMeshDescriptor *desc) {

  // init gizmo reference list
  const size_t gizmo_mesh_count = 3;
  mesh_reference_list_create(list, gizmo_mesh_count);

  // load arrow mesh binary
  Primitive mesh_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = desc->mbin_path,
      .primitive = &mesh_primitive,
  });

  // create new mesh in mesh ref list (x, y ,z)
  for (size_t i = 0; i < gizmo_mesh_count; i++) {
    Mesh *mesh = mesh_list_new_mesh(desc->list);

    // init mesh
    mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                     .primitive = mesh_primitive,
                                     .device = desc->device,
                                     .queue = desc->queue,
                                     .name = "translate arrow",
                                 });
    // add shader
    mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                               .path = SHADER_PATH_FLAT,
                               .device = desc->device,
                               .queue = desc->queue,
                               .label = "Gizmo transform translate shader",
                               .name = "Gizmo transform translate shader",
                           });

    // add color uniform
    shader_add_uniform(mesh_shader_texture(mesh),
                       &(ShaderCreateUniformDescriptor){
                           .entry_count = 1,
                           .group_index = 1,
                           .visibility = WGPUShaderStage_Fragment,
                           .entries =
                               (ShaderBindGroupUniformEntry[]){
                                   {
                                       .binding = 0,
                                       .size = sizeof(vec3),
                                       .data = &(vec3){i == 0, i == 1, i == 2},
                                       .offset = 0,
                                   },
                               },
                       });

    // set double sided culling
    material_texture_double_sided(mesh);
    // rotate
    mesh_rotate(mesh, (vec3){
                           (i == 2) * 90.0f,
                           0.0f,
                           (i == 0) * 90.0f,
                       });

    // update gizmo ref list
    mesh_reference_list_insert(list, mesh);
  }
}
