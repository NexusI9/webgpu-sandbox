#include "scale.h"

#include "./utils.h"
#include "core.h"
#include "runtime/mesh/core.h"

void gizmo_scale_create(MeshRefList *visual_list, MeshRefList *interactive_list,
                        const GizmoCreateDescriptor *desc) {

  gizmo_create_handles(visual_list, interactive_list,
                       &(GizmoCreateMeshDescriptor){
                           .list = desc->list,
                           .mbin_path = "./resources/assets/mbin/scale.mbin",
                           .offset = {0.0f, 0.1f, 0.0f},
                       });
}
