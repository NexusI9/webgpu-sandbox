#include "rotate.h"

#include "backend/resource_manager.h"
#include "resources/loader/loader.mbin.h"
#include "utils/color.h"
#include "./utils.h"
#include "runtime/mesh/list.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/primitive/core.h"
#include "core.h"
#include "runtime/mesh/core.h"

void gizmo_rotation_create(MeshRefList *visual_list,
                                   MeshRefList *interactive_list,
                                   const GizmoCreateDescriptor *desc) {

  mesh_ref_list_create(visual_list, 4);

  // sphere mask first
  Mesh *sphere = rem_new_mesh();
  Primitive sphere_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/rotate_mask.mbin",
      .primitive = &sphere_primitive,
  });

  gizmo_create_mesh(sphere, &sphere_primitive,
                              &(color){0.2f, 0.2f, 0.2f, 0.0f});

  mesh_ref_list_insert(visual_list, sphere);

  // free primitive
  primitive_destroy(&sphere_primitive);

  // create axis then
  gizmo_create_handles(
      visual_list, interactive_list,
      &(GizmoCreateMeshDescriptor){
          .mbin_path = "./resources/assets/mbin/rotate.mbin",
      });

}
