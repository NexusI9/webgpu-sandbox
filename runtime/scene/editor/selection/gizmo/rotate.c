#include "rotate.h"
#include "../resources/loader/loader.mbin.h"
#include "../utils/color.h"
#include "./utils.h"
#include "webgpu/webgpu.h"

void gizmo_transform_rotation_create(MeshRefList *visual_list,
                                   MeshRefList *interactive_list,
                                   const GizmoCreateDescriptor *desc) {

  mesh_ref_list_create(visual_list, 4);

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

  mesh_ref_list_insert(visual_list, sphere);

  // free primitive
  primitive_destroy(&sphere_primitive);

  // create axis then
  gizmo_transform_create_handles(
      visual_list, interactive_list,
      &(GizmoTransformCreateMeshDescriptor){
          .device = desc->device,
          .queue = desc->queue,
          .list = desc->list,
          .mbin_path = "./resources/assets/mbin/rotate.mbin",
      });


}
