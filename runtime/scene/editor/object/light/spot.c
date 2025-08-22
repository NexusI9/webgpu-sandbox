#include "spot.h"
#include "../builder/builder.h"
#include "../runtime/scene/editor/object/object.h"
#include "../runtime/scene/scene.h"
#include "./utils.h"

static inline void seo_light_spot_create_common(SceneEditorObject *,
                                                SpotLight *,
                                                const SEOCreateDescriptor *);

void seo_light_spot_create_common(SceneEditorObject *seo, SpotLight *light,
                                  const SEOCreateDescriptor *desc) {

  // define target
  seo->scene = desc->scene;

  // define mesh
  const size_t gizmo_mesh_count = 1;
  seo_mesh_list_create(&seo->meshes, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  SceneEditorObjectMesh *icon = seo_mesh_list_new_entry(&seo->meshes);
  icon->mesh = scene_new_mesh(desc->scene);
  icon->target = light;
  icon->target_list_index = desc->target_list_index;

  const char *texture_path = "./resources/assets/texture/ui/light-spot.png";

  // create gizmo mesh
  seo_create_billboard(icon->mesh, &(SEOCreateBillboardDescriptor){
                                       .texture_path = texture_path,
                                       .device = desc->device,
                                       .queue = desc->queue,
                                       .position = &light->position,
                                       .scale = &SEO_BILLBOARD_SCALE,
                                   });

  seo->origin = icon->mesh;
}

/**
   Insert Spot light gizmo mesh to the list
 */
void seo_light_spot_create(SceneEditorObject *seo, SpotLight *light,
                           const SEOCreateDescriptor *desc) {

  seo_light_spot_create_common(seo, light, desc);

  seo_light_spot_update_transform_callback(seo, LightShadow_None);
}

void seo_light_spot_set_position(SEOTransformCallback *desc) {

  SunLight *light = (SunLight *)desc->mesh->target;

  glm_vec3_copy(desc->offset, light->position);

  for (size_t i = 0; i < desc->seo->meshes.length; i++)
    mesh_set_position(desc->seo->meshes.entries[i].mesh, desc->offset);
}

void seo_light_spot_set_rotation(SEOTransformCallback *desc) {}

void seo_light_spot_set_scale(SEOTransformCallback *desc) {}

/* Shadow */

void seo_light_spot_shadow_create(SceneEditorObject *seo, SpotLight *light,
                                  const SEOCreateDescriptor *desc) {

  seo_light_spot_create_common(seo, light, desc);

  seo_light_spot_update_transform_callback(seo, LightShadow_Enabled);
}

void seo_light_spot_shadow_set_position(SEOTransformCallback *desc) {

  SpotLight *light = (SpotLight *)desc->mesh->target;

  glm_vec3_copy(desc->offset, light->position);

  for (size_t i = 0; i < desc->seo->meshes.length; i++)
    mesh_set_position(desc->seo->meshes.entries[i].mesh, desc->offset);

  // update light shadow map
  if (scene_renderer_draw_mode(&desc->seo->scene->renderer) ==
      SceneRendererDrawMode_Texture) {

    shadow_map_draw_spot_light(&(ShadowMapDrawSpotLightDescriptor){
        .light = light,
        .pass = &desc->seo->scene->lights.spot.shadow.pass,
        .device = scene_device(desc->seo->scene),
        .queue = scene_queue(desc->seo->scene),
        .layer = desc->mesh->target_list_index,
        .encoder = NULL,
    });

    seo_light_update_shadow_map(desc->seo->scene);
  }
}

void seo_light_spot_shadow_set_rotation(SEOTransformCallback *desc) {}

static const seo_transform_axis_callback
    light_transform_callback[2][GIZMO_TRANSFORM_MODE_COUNT] = {
        [LightShadow_None] =
            {
                [GizmoTransformMode_Position] = seo_light_spot_set_position,
                [GizmoTransformMode_Rotation] = seo_light_spot_set_rotation,
                [GizmoTransformMode_Scale] = seo_light_spot_set_scale,
            },
        [LightShadow_Enabled] =
            {
                [GizmoTransformMode_Position] =
                    seo_light_spot_shadow_set_position,
                [GizmoTransformMode_Rotation] =
                    seo_light_spot_shadow_set_rotation,
                [GizmoTransformMode_Scale] = seo_light_spot_set_scale,
            },
};

void seo_light_spot_update_transform_callback(SceneEditorObject *seo,
                                              const LightShadow shadow) {

  for (size_t i = 0; i < seo->meshes.length; i++)
    for (GizmoTransformMode j = 0; j < GIZMO_TRANSFORM_MODE_COUNT; j++)
      seo->meshes.entries[i].transform_callback[j] =
          light_transform_callback[shadow][i];
}
