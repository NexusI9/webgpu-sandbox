#include "translate.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/material/material.h"
#include "./transform.h"
#include "./utils.h"
#include "webgpu/webgpu.h"

void gizmo_transform_translate_create(MeshRefList *list,
                                      GizmoTransformMeshAxis *mesh_axis,
                                      const GizmoCreateDescriptor *desc) {

  gizmo_transform_create_handles(
      list, mesh_axis,
      &(GizmoTransformCreateMeshDescriptor){
          .device = desc->device,
          .queue = desc->queue,
          .list = desc->list,
          .mbin_path = "./resources/assets/mbin/translate.mbin",
      });
}
