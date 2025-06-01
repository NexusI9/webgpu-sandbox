#include "light_point.h"
#include "billboard.h"

/**
   Insert Point light gizmo mesh to the list
 */
void gizmo_light_point_create(PointLight *light,
                             const GizmoLightCreateDescriptor *desc) {

  Mesh *light_mesh = mesh_list_insert(desc->list);
  const char *texture_path = "./resources/assets/texture/ui/light-point.png";

  // create gizmo mesh
  gizmo_create_billboard(light_mesh, &(GizmoCreateBillboardDescriptor){
                                         .texture_path = texture_path,
                                         .camera = desc->camera,
                                         .viewport = desc->viewport,
                                         .device = desc->device,
                                         .queue = desc->queue,
                                         .position = &light->position,
                                         .scale = &(vec3){0.8f, 0.8f, 0.8f},
                                     });

  // cache pointer in destination
  mesh_reference_list_insert(desc->used_pointers, light_mesh);
  desc->used_pointers->length = 1;
  desc->used_pointers->capacity = 1;
}
