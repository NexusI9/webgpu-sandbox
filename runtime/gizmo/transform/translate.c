#include "translate.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/material/material.h"
#include "./transform.h"
#include "./utils.h"
#include "webgpu/webgpu.h"

void gizmo_transform_translate_create(MeshRefList *list,
                                      const GizmoCreateDescriptor *desc) {

  gizmo_transform_create_mesh(
      list, &(GizmoTransformCreateMeshDescriptor){
                 .device = desc->device,
                 .queue = desc->queue,
                 .list = desc->list,
                 .mbin_path = "./resources/assets/mbin/translate.mbin",
             });
}

void gizmo_transform_translate_translate(MeshRefList *gizmo, vec3 translation) {
  mesh_reference_list_translate(gizmo, translation);
}

void gizmo_transform_translate_rotate(MeshRefList *gizmo, vec3 rotation) {
  mesh_reference_list_rotate(gizmo, rotation);
}
