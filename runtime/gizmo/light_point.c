#include "light_point.h"
#include "../../resources/loader/loader.mbin.h"
#include "billboard.h"
#include "wireframe.h"

/**
   Insert Point light gizmo mesh to the list
 */
void gizmo_light_point_create(GizmoPointLight *gizmo, PointLight *light,
                              const GizmoCreateDescriptor *desc) {

  // define target
  gizmo->target = light;

  // define mesh
  size_t gizmo_mesh_count = 2;
  mesh_reference_list_create(&gizmo->meshes, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  Mesh *icon = mesh_list_new_mesh(desc->list);
  const char *texture_path = "./resources/assets/texture/ui/light-point.png";

  // create gizmo mesh
  gizmo_create_billboard(icon, &(GizmoCreateBillboardDescriptor){
                                   .texture_path = texture_path,
                                   .device = desc->device,
                                   .queue = desc->queue,
                                   .position = &light->position,
                                   .scale = &GIZMO_BILLBOARD_SCALE,
                               });

  // store mesh pointer in gizmo ref list
  mesh_reference_list_insert(&gizmo->meshes, icon);

  // create sphere
  Mesh *sphere = mesh_list_new_mesh(desc->list);
  Primitive sphere_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .primitive = &sphere_primitive,
      .path = "./resources/assets/mbin/sphere.mbin",
  });

  gizmo_create_wireframe(sphere, &(GizmoCreateWireframeDescriptor){
                                     .color = &(vec3){0.4f, 0.8f, 1.0f},
                                     .device = desc->device,
                                     .queue = desc->queue,
                                     .name = "Gizmo spot light sphere",
                                     .index = &sphere_primitive.index,
                                     .vertex = &sphere_primitive.vertex,
                                     .thickness = 0.005f,
                                 });

  // scale sphere to point far point
  mesh_scale(sphere, (vec3){light->far, light->far, light->far});

  mesh_reference_list_insert(&gizmo->meshes, sphere);
}
