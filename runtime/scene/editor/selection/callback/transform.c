#include "transform.h"

/**

   ▗▖  ▗▖▗▄▄▄▖ ▗▄▄▖▗▖ ▗▖
   ▐▛▚▞▜▌▐▌   ▐▌   ▐▌ ▐▌
   ▐▌  ▐▌▐▛▀▀▘ ▝▀▚▖▐▛▀▜▌
   ▐▌  ▐▌▐▙▄▄▖▗▄▄▞▘▐▌ ▐▌

 */
static const mesh_transform_callback transform_callback_mesh[] = {
    [GizmoTransformMode_Position] = mesh_set_position,
    [GizmoTransformMode_Rotation] = mesh_set_rotation,
    [GizmoTransformMode_Scale] = mesh_set_scale,
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
                                   seo_transform_axis_callback,
                                   SceneSelectionTransform *);

void scene_selection_seo_transform_core(
    Mesh *mesh, SceneEditorObject *seo, vec3 *init_attribute,
    seo_transform_axis_callback transform_callback,
    SceneSelectionTransform *desc) {

  // printf("%s\n", seo->meshes.entries[i].mesh->name);
  //  calculate offset from delta
  vec3 offset_attribute;
  glm_vec3_add(*init_attribute, *desc->delta, offset_attribute);

  // transform seo via their own callback
  transform_callback(mesh, seo, offset_attribute);
}

/*
  SEO based transform

   .--------------------------------------------------------------.
   |                       Actives Meshes                         |
   |------------------------------.-------------------------------|
   | Mesh 1 |  Mesh 2  |  Mesh 3  |  Mesh 1  |  Mesh 2  | Mesh 3  |
   |------------------------------+-------------------------------|
   |           SEO 1              |            SEO 2              |
   '------------------------------'-------------------------------'

 */
void scene_selection_seo_transform(SceneSelectionTransform *desc) {

  size_t offset = 0;

  for (size_t i = 0; i < desc->active_meshes->length; i++) {

    vec3 *init_attribute = &desc->initial_attributes->entries[i];
    Mesh *mesh = desc->active_meshes->entries[i];

    SceneEditorObject *seo = (SceneEditorObject *)desc->target_list->entries[i];

    /* For now each SEO mesh has its own entry
     However the selection only works with a flat array of mesh, thus we need
     to map back the seo meshes using an offset.

     It is still unsure if the "per mesh callback" is necessary, however it
     provides for sure more flexbility for more complex SEO, so it's been
     decided to keep it for now. However if after implementing more complex SEO
     (like target spot lights, camera look at target) it proves to not really be
     necessary then it's possible to implement back to a more simple/ "per seo"
     transform callback.
     */
    size_t local_index = i - offset;
    if (local_index >= seo->meshes.length) {
      offset += seo->meshes.length;
      local_index = 0;
    }

    seo_transform_axis_callback transform_callback =
        seo->meshes.entries[local_index]
            .transform_callback[desc->transform_mode];

    scene_selection_seo_transform_core(mesh, seo, init_attribute,
                                       transform_callback, desc);
  }
}


