#include "rotate.h"
#include "./utils.h"


void gizmo_transform_rotate_create(MeshRefList *list,
                                   const GizmoCreateDescriptor *desc) {

  gizmo_transform_create_mesh(
      list, &(GizmoTransformCreateMeshDescriptor){
                .device = desc->device,
                .queue = desc->queue,
                .list = desc->list,
                .mbin_path = "./resources/assets/mbin/rotate.mbin",
            });
}
