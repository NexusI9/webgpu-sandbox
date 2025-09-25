#include "translate.h"

#include "./utils.h"
#include "core.h"
#include "runtime/mesh/core.h"

void gizmo_position_create(MeshRefList *list, MeshRefList *interactive_list,
                           const GizmoCreateDescriptor *desc) {

  gizmo_create_handles(
      list, interactive_list,
      &(GizmoCreateMeshDescriptor){
          .list = desc->list,
          .mbin_path = "./resources/assets/mbin/translate.mbin",
      });
}
