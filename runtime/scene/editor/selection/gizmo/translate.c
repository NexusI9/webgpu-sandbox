#include "translate.h"
#include "../resources/loader/loader.mbin.h"
#include "./utils.h"
#include "webgpu/webgpu.h"

void gizmo_position_create(MeshRefList *list, MeshRefList *interactive_list,
                           const GizmoCreateDescriptor *desc) {

  gizmo_create_handles(
      list, interactive_list,
      &(GizmoCreateMeshDescriptor){
          .device = desc->device,
          .queue = desc->queue,
          .list = desc->list,
          .mbin_path = "./resources/assets/mbin/translate.mbin",
      });
}
