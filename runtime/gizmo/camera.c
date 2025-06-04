#include "camera.h"
#include "../resources/loader/loader.mbin.h"
#include "billboard.h"
#include "wireframe.h"

void gizmo_camera_create(GizmoCamera *gizmo, Camera *camera,
                         const GizmoCreateDescriptor *desc) {

  // define target
  gizmo->target = camera;

  size_t gizmo_mesh_count = 2;
  mesh_reference_list_create(&gizmo->meshes, gizmo_mesh_count);

  // create new mesh in the mesh list
  Mesh *icon = mesh_list_new_mesh(desc->list);
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

  // create box mesh
  Primitive cube_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/cube.mbin",
      .primitive = &cube_primitive,
  });
  
  Mesh *cube = mesh_list_new_mesh(desc->list);

  gizmo_create_wireframe(cube, &(GizmoCreateWireframeDescriptor){
                                   .camera = desc->camera,
                                   .viewport = desc->viewport,
                                   .device = desc->device,
                                   .queue = desc->queue,
                                   .color = &(vec3){0.0f, 1.0f, 0.0f},
                                   .thickness = GIZMO_WIREFRAME_LINE_THICKNESS,
                                   .side = 1.0f,
                                   .vertex = &cube_primitive.vertex,
                                   .index = &cube_primitive.index,
                                   .name = "gizmo camera",
                               });

  mesh_reference_list_insert(&gizmo->meshes, cube);
  // update pointer list length
  gizmo->meshes.length = gizmo_mesh_count;
}

void gizmo_camera_translate(GizmoCamera *gizmo, vec3 position) {

  // transform target
  camera_translate(gizmo->target, position);

  // transform mesh
  mesh_reference_list_translate(&gizmo->meshes, position);
}

void gizmo_camera_rotate(GizmoCamera *gizmo, vec3 rotation) {}
void gizmo_camera_scale(GizmoCamera *gizmo, vec3 scale) {}
void gizmo_camera_lookat(GizmoCamera *gizmo, vec3 position) {}
