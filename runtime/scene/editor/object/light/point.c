#include "./point.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/scene/scene.h"
#include "./utils.h"

static inline void seo_light_point_create_common(SceneEditorObject *,
                                                 PointLight *,
                                                 const SEOCreateDescriptor *);

void seo_light_point_create_common(SceneEditorObject *seo, PointLight *light,
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
  const char *texture_path = "./resources/assets/texture/ui/light-point.png";

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
}

/**
   Insert Point light gizmo mesh to the list
 */
void seo_light_point_create(SceneEditorObject *seo, PointLight *light,
                            const SEOCreateDescriptor *desc) {

  seo_light_point_create_common(seo, light, desc);

  seo_light_point_update_transform_callback(seo, LightShadow_None);
}

void seo_light_point_translate(SceneEditorObject *seo, vec3 value) {

  PointLight *light = (PointLight *)seo->target;

  glm_vec3_copy(value, light->position);

  mesh_ref_list_translate(&seo->meshes, value);
}

void seo_light_point_rotate(SceneEditorObject *seo, vec3 value) {}

void seo_light_point_scale(SceneEditorObject *seo, vec3 value) {}

/* shadow */
void seo_light_point_shadow_create(SceneEditorObject *seo, PointLight *light,
                                   const SEOCreateDescriptor *desc) {

  seo_light_point_create_common(seo, light, desc);

  seo_light_point_update_transform_callback(seo, LightShadow_Enabled);
}

void seo_light_point_shadow_translate(SceneEditorObject *seo, vec3 value) {

  PointLight *light = (PointLight *)seo->target;

  glm_vec3_copy(value, light->position);

  mesh_ref_list_translate(&seo->meshes, value);

  // update light shadow map
  if (scene_renderer_draw_mode(&seo->scene->renderer) ==
      SceneRendererDrawMode_Texture) {

    shadow_map_draw_point_light(&(ShadowMapDrawPointLightDescriptor){
        .light =
            seo->scene->lights.point.shadow.entries[seo->target_list_index],
        .mesh_list =
            scene_pipeline(seo->scene, ScenePipeline_Dynamic_LitShadow),
        .color_map = seo->scene->lights.point.shadow.color_map,
        .depth_map = seo->scene->lights.point.shadow.depth_map,
        .device = scene_device(seo->scene),
        .queue = scene_queue(seo->scene),
        .layer = seo->target_list_index,
        .encoder = NULL,
    });

    seo_light_update_shadow_map(seo->scene);
  }
}

static const seo_transform_axis_callback
    light_transform_callback[2][GIZMO_TRANSFORM_MODE_COUNT] = {
        [LightShadow_None] =
            {
                [GizmoTransformMode_Translate] = seo_light_point_translate,
                [GizmoTransformMode_Rotate] = seo_light_point_rotate,
                [GizmoTransformMode_Scale] = seo_light_point_scale,
            },
        [LightShadow_Enabled] =
            {
                [GizmoTransformMode_Translate] =
                    seo_light_point_shadow_translate,
                [GizmoTransformMode_Rotate] = seo_light_point_rotate,
                [GizmoTransformMode_Scale] = seo_light_point_scale,
            },
};

void seo_light_point_update_transform_callback(SceneEditorObject *seo,
                                               const LightShadow shadow) {

  for (GizmoTransformMode i = 0; i < GIZMO_TRANSFORM_MODE_COUNT; i++)
    seo->transform_callback[i] = light_transform_callback[shadow][i];
}
