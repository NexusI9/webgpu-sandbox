#include "rotate.h"
#include "../resources/loader/loader.mbin.h"
#include "../utils/color.h"
#include "./utils.h"
#include "webgpu/webgpu.h"

void gizmo_transform_rotate_create(MeshRefList *list,
                                   GizmoTransformMeshAxis *mesh_axis,
                                   const GizmoCreateDescriptor *desc) {

  mesh_ref_list_create(list, 4);

  // sphere mask first
  Mesh *sphere = mesh_list_new_mesh(desc->list);
  Primitive sphere_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/rotate_mask.mbin",
      .primitive = &sphere_primitive,
  });

  gizmo_transform_create_mesh(sphere, &sphere_primitive,
                              &(color){0.2f, 0.2f, 0.2f, 0.0f}, desc->queue,
                              desc->device);

  // occlude
  pipeline_set_stencil(shader_pipeline(mesh_shader_texture(sphere)),
                       (WGPUDepthStencilState){
                           .depthWriteEnabled = true,
                           .depthCompare = WGPUCompareFunction_Less,
                           .format = WGPUTextureFormat_Depth24Plus,
                       });

  mesh_ref_list_insert(list, sphere);

  // create axis then
  gizmo_transform_create_handles(
      list, mesh_axis,
      &(GizmoTransformCreateMeshDescriptor){
          .device = desc->device,
          .queue = desc->queue,
          .list = desc->list,
          .mbin_path = "./resources/assets/mbin/rotate.mbin",
      });
}
