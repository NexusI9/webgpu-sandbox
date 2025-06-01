#include "camera.h"
#include "billboard.h"

void gizmo_camera_create(GizmoCamera *gizmo, Camera *camera,
                         const GizmoCreateDescriptor *desc) {

  // define target
  gizmo->target = camera;

  size_t gizmo_mesh_count = 1;
  mesh_reference_list_create(&gizmo->meshes, gizmo_mesh_count);

  // create new mesh in the mesh list
  Mesh *icon = mesh_list_insert(desc->list);
  const char *texture_path = "./resources/assets/texture/ui/camera.png";

  // create icon mesh
  gizmo_create_billboard(icon, &(GizmoCreateBillboardDescriptor){
                                   .texture_path = texture_path,
                                   .camera = desc->camera,
                                   .viewport = desc->viewport,
                                   .device = desc->device,
                                   .queue = desc->queue,
                                   .position = &camera->position,
                                   .scale = &(vec3){0.8f, 0.8f, 0.8f},
                               });

  // store mesh pointer in gizmo mesh ref list
  mesh_reference_list_insert(&gizmo->meshes, icon);

  // update pointer list length
  gizmo->meshes.length = gizmo_mesh_count;
}

void gizmo_camera_translate(GizmoCamera *gizmo, vec3 position) {}
void gizmo_camera_rotate(GizmoCamera *gizmo, vec3 rotation) {}
void gizmo_camera_scale(GizmoCamera *gizmo, vec3 scale) {}
void gizmo_camera_lookat(GizmoCamera *gizmo, vec3 position) {}
