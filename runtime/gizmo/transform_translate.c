#include "transform_translate.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/material/material.h"
#include "./transform.h"
#include "webgpu/webgpu.h"

void gizmo_transform_translate_create(GizmoTransformTranslate *gizmo,
                                      const GizmoCreateDescriptor *desc) {

  gizmo_transform_create_mesh(
      &gizmo->meshes, &(GizmoTransformCreateMeshDescriptor){
                          .device = desc->device,
                          .queue = desc->queue,
                          .list = desc->list,
                          .mbin_path = "./resources/assets/mbin/translate.mbin",
                      });
}

void gizmo_transform_translate_translate(GizmoTransformTranslate *gizmo,
                                         vec3 translation) {
  mesh_reference_list_translate(&gizmo->meshes, translation);
}

void gizmo_transform_translate_rotate(GizmoTransformTranslate *gizmo,
                                      vec3 rotation) {
  mesh_reference_list_rotate(&gizmo->meshes, rotation);
}
