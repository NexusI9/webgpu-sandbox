#include "light_ambient.h"
#include "billboard.h"

/**
   Insert Ambient light gizmo mesh to the list
 */
void gizmo_light_ambient_create(GizmoAmbientLight *gizmo, AmbientLight *light,
                                const GizmoCreateDescriptor *desc) {

  // define target
  gizmo->target = light;

  // define mesh
  size_t gizmo_mesh_count = 1;
  mesh_reference_list_create(&gizmo->meshes, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  Mesh *icon = mesh_list_new_mesh(desc->list);
  const char *texture_path = "./resources/assets/texture/ui/light-ambient.png";

  // create gizmo mesh
  gizmo_create_billboard(icon, &(GizmoCreateBillboardDescriptor){
                                   .texture_path = texture_path,
                                   .camera = desc->camera,
                                   .viewport = desc->viewport,
                                   .device = desc->device,
                                   .queue = desc->queue,
                                   .position = &light->position,
                                   .scale = &(vec3){0.8f, 0.8f, 0.8f},
                               });

  // store mesh pointer in gizmo ref list
  mesh_reference_list_insert(&gizmo->meshes, icon);
  // update pointer list length
  gizmo->meshes.length = gizmo_mesh_count;
  
}
