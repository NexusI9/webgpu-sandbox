#include "transform_translate.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/material/material.h"
#include "webgpu/webgpu.h"

void gizmo_transform_translate_create(GizmoTransformTranslate *gizmo,
                                      const GizmoCreateDescriptor *desc) {

  // init gizmo reference list
  const size_t gizmo_mesh_count = 3;
  mesh_reference_list_create(&gizmo->meshes, gizmo_mesh_count);

  // load arrow mesh binary
  Primitive arrow_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/translate.mbin",
      .primitive = &arrow_primitive,
  });

  // create new mesh in mesh ref list (x, y ,z)
  for (size_t i = 0; i < gizmo_mesh_count; i++) {
    Mesh *arrow = mesh_list_new_mesh(desc->list);

    // init mesh
    mesh_create_primitive(arrow, &(MeshCreatePrimitiveDescriptor){
                                     .primitive = arrow_primitive,
                                     .device = desc->device,
                                     .queue = desc->queue,
                                     .name = "translate arrow",
                                 });
    // add shader
    mesh_set_shader(arrow, &(ShaderCreateDescriptor){
                               .path = SHADER_PATH_FLAT,
                               .device = desc->device,
                               .queue = desc->queue,
                               .label = "Gizmo transform translate shader",
                               .name = "Gizmo transform translate shader",
                           });

    // add color uniform
    shader_add_uniform(mesh_shader_texture(arrow),
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
    material_texture_double_sided(arrow);
    // rotate
    mesh_rotate(arrow, (vec3){
                           (i == 2) * 90.0f,
                           0.0f,
                           (i == 0) * 90.0f,
                       });

    // update gizmo ref list
    mesh_reference_list_insert(&gizmo->meshes, arrow);
  }
}

void gizmo_transform_translate_translate(GizmoTransformTranslate *gizmo,
                                         vec3 translation) {
  mesh_reference_list_translate(&gizmo->meshes, translation);
}

void gizmo_transform_translate_rotate(GizmoTransformTranslate *gizmo,
                                      vec3 rotation) {
  mesh_reference_list_rotate(&gizmo->meshes, rotation);
}
