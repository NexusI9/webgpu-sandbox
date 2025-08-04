#include "sun.h"
#include "../builder/builder.h"

/**
   Insert Sun light gizmo mesh to the list
 */
void seo_light_sun_create(SceneEditorObject *gizmo, SunLight *light,
                            const GizmoCreateDescriptor *desc) {

  // define target
  gizmo->target = light;

  // define mesh
  size_t gizmo_mesh_count = 1;
  mesh_ref_list_create(&gizmo->meshes, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  Mesh *icon = mesh_list_new_mesh(desc->list);
  const char *texture_path = "./resources/assets/texture/ui/light-sun.png";

  // create gizmo mesh
  seo_create_billboard(icon, &(SEOCreateBillboardDescriptor){
                                   .texture_path = texture_path,
                                   .device = desc->device,
                                   .queue = desc->queue,
                                   .position = &light->position,
                                   .scale = &SEO_BILLBOARD_SCALE,
                               });

  // store mesh pointer in gizmo ref list
  mesh_ref_list_insert(&gizmo->meshes, icon);
}
