#include "transform.h"

/**

   ▗▖  ▗▖▗▄▄▄▖ ▗▄▄▖▗▖ ▗▖
   ▐▛▚▞▜▌▐▌   ▐▌   ▐▌ ▐▌
   ▐▌  ▐▌▐▛▀▀▘ ▝▀▚▖▐▛▀▜▌
   ▐▌  ▐▌▐▙▄▄▖▗▄▄▞▘▐▌ ▐▌

 */
static const mesh_transform_callback transform_callback_mesh[] = {
    [GizmoTransformMode_Translate] = mesh_translate,
    [GizmoTransformMode_Rotate] = mesh_rotate,
    [GizmoTransformMode_Scale] = mesh_scale,
};

static inline void
scene_selection_mesh_transform_core(Mesh *, vec3 *, SceneSelectionTransform *);

void scene_selection_mesh_transform_core(Mesh *mesh, vec3 *init_attribute,
                                         SceneSelectionTransform *desc) {

  // calculate offset from delta
  vec3 offset_attribute;
  glm_vec3_add(*init_attribute, *desc->delta, offset_attribute);

  // transform mesh
  transform_callback_mesh[desc->transform_mode](mesh, offset_attribute);
}

/* Mesh based transform */
void scene_selection_mesh_transform(SceneSelectionTransform *desc) {

  for (size_t i = 0; i < desc->active_meshes->length; i++) {
    Mesh *mesh = desc->active_meshes->entries[i];
    vec3 *init_attribute = &desc->initial_attributes->entries[i];

    // transform mesh
    scene_selection_mesh_transform_core(mesh, init_attribute, desc);
  }
}

/* Mesh shadow based transform.
   Note that this filter only incudes meshes that are in the LitShadow pipelines
 */
void scene_selection_mesh_shadow_transform(SceneSelectionTransform *desc) {

  for (size_t i = 0; i < desc->active_meshes->length; i++) {
    Mesh *mesh = desc->active_meshes->entries[i];
    vec3 *init_attribute = &desc->initial_attributes->entries[i];

    // transform mesh
    scene_selection_mesh_transform_core(mesh, init_attribute, desc);

    // update mesh shadow model uniform
    mesh_shader_shadow_update_model(mesh);
  }

  // recalculate shadow maps
  if (desc->scene->renderer.draw.mode == SceneRendererDrawMode_Texture)
    shadow_map_draw_all(&(ShadowMapDrawAllDescriptor){
        .device = scene_device(desc->scene),
        .queue = scene_queue(desc->scene),
        .mesh_list =
            scene_pipeline(desc->scene, ScenePipeline_Dynamic_LitShadow),
        .lights = &desc->scene->lights,
    });
}

/**

    ▗▄▄▖▗▄▄▄▖ ▗▄▖
   ▐▌   ▐▌   ▐▌ ▐▌
    ▝▀▚▖▐▛▀▀▘▐▌ ▐▌
   ▗▄▄▞▘▐▙▄▄▖▝▚▄▞▘

 */

static inline void
scene_selection_seo_transform_core(Mesh *, SceneEditorObject *, vec3 *,
                                   SceneSelectionTransform *);

void scene_selection_seo_transform_core(Mesh *mesh, SceneEditorObject *seo,
                                        vec3 *init_attribute,
                                        SceneSelectionTransform *desc) {

  // calculate offset from delta
  vec3 offset_attribute;
  glm_vec3_add(*init_attribute, *desc->delta, offset_attribute);

  // transform seo via their own callback
  seo->transform_callback[desc->transform_mode](seo, offset_attribute);
}

/* SEO based transform */
void scene_selection_seo_transform(SceneSelectionTransform *desc) {
  for (size_t i = 0; i < desc->active_meshes->length; i++) {

    vec3 *init_attribute = &desc->initial_attributes->entries[i];
    Mesh *mesh = desc->active_meshes->entries[i];
    SceneEditorObject *seo = (SceneEditorObject *)desc->target_list->entries[i];
    
    scene_selection_seo_transform_core(mesh, seo, init_attribute, desc);
  }
}

/* SEO Shadow based transform */
void scene_selection_seo_shadow_transform(SceneSelectionTransform *desc) {

  for (size_t i = 0; i < desc->active_meshes->length; i++) {

    vec3 *init_attribute = &desc->initial_attributes->entries[i];
    Mesh *mesh = desc->active_meshes->entries[i];
    SceneEditorObject *seo = (SceneEditorObject *)desc->target_list->entries[i];

    scene_selection_seo_transform_core(mesh, seo, init_attribute, desc);
  }
}
