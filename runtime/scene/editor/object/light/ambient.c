#include "./ambient.h"
#include "../builder/builder.h"
#include "../runtime/scene/scene.h"

/**
   Insert Ambient light gizmo mesh to the list
 */
void seo_light_ambient_create(SceneEditorObject *seo, AmbientLight *light,
                              const SEOCreateDescriptor *desc) {

  // define target
  seo->target = light;

  // define mesh
  size_t gizmo_mesh_count = 1;
  mesh_ref_list_create(&seo->meshes, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  Mesh *icon = scene_new_mesh(desc->scene);
  const char *texture_path = "./resources/assets/texture/ui/light-ambient.png";

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
      seo_light_ambient_translate;
  seo->transform_callback[GizmoTransformMode_Rotate] = seo_light_ambient_rotate;
  seo->transform_callback[GizmoTransformMode_Scale] = seo_light_ambient_scale;

  seo->scene = desc->scene;
}

void seo_light_ambient_translate(SceneEditorObject *seo, vec3 value) {

  mesh_ref_list_translate(&seo->meshes, value);
}

void seo_light_ambient_rotate(SceneEditorObject *seo, vec3 value) {}

void seo_light_ambient_scale(SceneEditorObject *seo, vec3 value) {}
