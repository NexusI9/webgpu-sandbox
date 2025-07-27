#include "utils.h"
#include "../../../resources/loader/loader.mbin.h"
#include "../../material/material.h"
#include "./core.h"
#include "webgpu/webgpu.h"

/**
   Create a gizmo transform mesh with the solid pipeline and the other relative
   pipeline settings (no depth write).

   Used to generate each gizmo handles.
 */
void gizmo_transform_create_mesh(Mesh *mesh, Primitive *primitive,
                                 const color *rgba, const WGPUQueue *queue,
                                 const WGPUDevice *device) {

  // init mesh
  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = *primitive,
                                  .device = device,
                                  .queue = queue,
                                  .name = "Gizmo transform",
                              });
  // add shader
  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .path = SHADER_PATH_FLAT,
                            .device = device,
                            .queue = queue,
                            .label = "Gizmo transform shader",
                            .name = "Gizmo transform shader",
                        });

  // add color uniform
  const float fixed_size = GIZMO_TRANSFORM_SIZE;
  shader_add_uniform(mesh_shader_texture(mesh),
                     &(ShaderCreateUniformDescriptor){
                         .entry_count = 2,
                         .group_index = 1,
                         .visibility = WGPUShaderStage_Fragment,
                         .entries =
                             (ShaderBindGroupUniformEntry[]){
                                 {
                                     .binding = 0,
                                     .size = sizeof(color),
                                     .data = (void *)rgba,
                                     .offset = 0,
                                 },
                                 {
                                     .binding = 1,
                                     .size = sizeof(float),
                                     .data = (void *)&fixed_size,
                                     .offset = 0,
                                 },
                             },
                     });

  // disable depth write
  pipeline_set_stencil(shader_pipeline(mesh_shader_texture(mesh)),
                       (WGPUDepthStencilState){
                           .depthWriteEnabled = true,
                           .depthCompare = WGPUCompareFunction_Less,
                           .format = WGPUTextureFormat_Depth24Plus,
                       });

  // set double sided culling
  // material_texture_double_sided(mesh);

  // scale gizmo (cpu side as well, so the hitbox are correct dimension)
  const float gizmo_size = 1.0f;
  mesh_scale(mesh, (vec3){gizmo_size, gizmo_size, gizmo_size});
}

/**
   Load the transform gizmom meshbinary and automate the shader/ color and angle
   process.
 */
void gizmo_transform_create_handles(
    MeshRefList *list, const GizmoTransformCreateMeshDescriptor *desc) {

  // init gizmo reference list
  const size_t gizmo_mesh_count = 3;
  if (list->entries == NULL)
    mesh_ref_list_create(list, gizmo_mesh_count);

  // load arrow mesh binary
  Primitive mesh_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = desc->mbin_path,
      .primitive = &mesh_primitive,
  });

  // create new mesh in mesh ref list (x, y ,z)
  for (size_t i = 0; i < gizmo_mesh_count; i++) {
    Mesh *mesh = mesh_list_new_mesh(desc->list);
    color rgba = {i == 0, i == 1, i == 2, 1.0f};

    gizmo_transform_create_mesh(mesh, &mesh_primitive, &rgba, desc->queue,
                                desc->device);

    // rotate
    mesh_rotate(mesh, (vec3){
                          (i == 2) * 90.0f,
                          0.0f,
                          (i == 0) * 90.0f,
                      });

    // update gizmo ref list
    mesh_ref_list_insert(list, mesh);
  }
}

void gizmo_transform_origin(GizmoTransform *gizmo, vec3 *position) {
  glm_vec3_copy(gizmo->handles[gizmo->mode].entries[0]->position, *position);
}
