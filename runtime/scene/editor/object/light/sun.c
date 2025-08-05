#include "sun.h"
#include "../builder/builder.h"
#include "../runtime/scene/scene.h"

/**
   Insert Sun light gizmo mesh to the list
 */
void seo_light_sun_create(SceneEditorObject *seo, SunLight *light,
                          const SEOCreateDescriptor *desc) {

  // define target
  seo->target = light;
  seo->scene = desc->scene;
  seo->target_list_index = desc->target_list_index;
  
  // define mesh
  size_t gizmo_mesh_count = 1;
  mesh_ref_list_create(&seo->meshes, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  Mesh *icon = scene_new_mesh(desc->scene);
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
  mesh_ref_list_insert(&seo->meshes, icon);

  // set callback
  seo->transform_callback[GizmoTransformMode_Translate] =
      seo_light_sun_translate;
  seo->transform_callback[GizmoTransformMode_Rotate] = seo_light_sun_rotate;
  seo->transform_callback[GizmoTransformMode_Scale] = seo_light_sun_scale;
}

void seo_light_sun_translate(SceneEditorObject *seo, vec3 value) {

  mesh_ref_list_translate(&seo->meshes, value);
}

void seo_light_sun_rotate(SceneEditorObject *seo, vec3 value) {}

void seo_light_sun_scale(SceneEditorObject *seo, vec3 value) {}
