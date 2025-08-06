#include "translate.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/material/material.h"
#include "./utils.h"
#include "webgpu/webgpu.h"

void gizmo_transform_translate_create(MeshRefList *list,
                                      MeshRefList *interactive_list,
                                      const GizmoCreateDescriptor *desc) {

  gizmo_transform_create_handles(
      list, interactive_list,
      &(GizmoTransformCreateMeshDescriptor){
          .device = desc->device,
          .queue = desc->queue,
          .pipeline = desc->pipeline,
          .list = desc->list,
          .mbin_path = "./resources/assets/mbin/translate.mbin",
      });
}
