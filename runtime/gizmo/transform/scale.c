#include "scale.h"
#include "./utils.h"

void gizmo_transform_scale_create(MeshRefList *visual_list,
                                  MeshRefList *interactive_list,
                                  const GizmoCreateDescriptor *desc) {

  gizmo_transform_create_handles(
      visual_list, interactive_list,
      &(GizmoTransformCreateMeshDescriptor){
          .device = desc->device,
          .queue = desc->queue,
          .list = desc->list,
          .mbin_path = "./resources/assets/mbin/scale.mbin",
      });
}
