#include "sun.h"
#include "../builder/builder.h"
#include "../runtime/scene/scene.h"
#include "./utils.h"

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

  SunLight *light = (SunLight *)seo->target;

  glm_vec3_copy(value, light->position);

  mesh_ref_list_translate(&seo->meshes, value);

   // update light shadow map
  if (scene_renderer_draw_mode(&seo->scene->renderer) ==
      SceneRendererDrawMode_Texture) {

    shadow_map_draw_sun_light(&(ShadowMapDrawSunLightDescriptor){
        .light = &seo->scene->lights.sun.entries[seo->target_list_index],
        .mesh_list =
            scene_pipeline(seo->scene, ScenePipeline_Dynamic_LitShadow),
        .color_map = seo->scene->lights.spot.color_map,
        .depth_map = seo->scene->lights.spot.depth_map,
        .device = scene_device(seo->scene),
        .queue = scene_queue(seo->scene),
        .layer = seo->target_list_index,
        .encoder = NULL,
    });

    seo_light_update_shadow_map(seo->scene);
  }
}

void seo_light_sun_rotate(SceneEditorObject *seo, vec3 value) {}

void seo_light_sun_scale(SceneEditorObject *seo, vec3 value) {}
