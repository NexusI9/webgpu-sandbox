#include "transform.h"

#include <cglm/types.h>
#include <cglm/vec3.h>
#include <stdbool.h>
#include <stddef.h>

#include "runtime/geometry/aabb/aabb.h"
#include "runtime/light/shadow_map/draw.h"
#include "runtime/mesh/transform.h"
#include "runtime/mesh/uniform.h"
#include "runtime/scene/debug/core.h"
#include "runtime/scene/renderer/core.h"
#include "backend/ssbo.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/selection/gizmo/core.h"

/**

   ▗▖  ▗▖▗▄▄▄▖ ▗▄▄▖▗▖ ▗▖
   ▐▛▚▞▜▌▐▌   ▐▌   ▐▌ ▐▌
   ▐▌  ▐▌▐▛▀▀▘ ▝▀▚▖▐▛▀▜▌
   ▐▌  ▐▌▐▙▄▄▖▗▄▄▞▘▐▌ ▐▌

 */
static const mesh_transform_callback transform_callback_mesh[] = {
    [GizmoMode_Position] = mesh_set_position,
    [GizmoMode_Rotation] = mesh_set_rotation,
    [GizmoMode_Scale] = mesh_set_scale,
};

static inline void
scene_selection_mesh_transform_core(Mesh *, vec3 *, SceneSelectionTransform *);

/**
   Update the mesh probes uniform (planar and grid) if the mesh goes within or
   out of the probe bounds/radius
 */
void scene_selection_mesh_update_probe_uniform(
    Mesh *mesh, ProbeReflectionGridList *grid_list,
    ProbeReflectionPlaneList *plane_list, SSBOManager *ssbo) {
  size_t i = 0;

  MeshUniform *uniform = mesh_uniform(mesh);

  for (i = 0; i < plane_list->length; i++) {

    ProbeReflectionPlane *probe = &plane_list->entries[i];
    bool intersect =
        aabb_intersect(&mesh->topology.boundbox.world, &probe->boundbox);

    if (intersect)
      mesh_uniform_set_probe_reflection_plane(mesh, ssbo);
    else
      mesh_uniform_clear_probe_reflection_plane(mesh, ssbo);
  }

  for (i = 0; i < grid_list->length; i++) {

    ProbeReflectionGrid *grid = &grid_list->entries[i];
    bool intersect =
        aabb_intersect(&mesh->topology.boundbox.world, &grid->boundbox);

    // if (intersect)
  }
}

void scene_selection_mesh_transform_core(Mesh *mesh, vec3 *init_attribute,
                                         SceneSelectionTransform *desc) {

  // calculate offset from delta
  vec3 offset_attribute;
  glm_vec3_add(*init_attribute, *desc->delta, offset_attribute);

  // transform mesh
  transform_callback_mesh[desc->transform_mode](mesh, offset_attribute);

  ssbo_update_queue_insert(&desc->scene->renderer.ssbo, SSBOType_Mesh,
                           mesh->ssbo_slot.id);

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
  }

  // recalculate shadow maps
  if (desc->scene->renderer.draw.mode == SceneRendererDrawMode_Texture)
    shadow_map_draw_all(
        &(ShadowMapDrawAllDescriptor){
            .device = scene_device(desc->scene),
            .queue = scene_queue(desc->scene),
            .mesh_list =
                scene_pipeline(desc->scene, ScenePipeline_Dynamic_LitShadow),
            .lights = &desc->scene->lights,
        },
        SCENE_DEBUG_UNDEFINED);
}

/**

    ▗▄▄▖▗▄▄▄▖ ▗▄▖
   ▐▌   ▐▌   ▐▌ ▐▌
    ▝▀▚▖▐▛▀▀▘▐▌ ▐▌
   ▗▄▄▞▘▐▙▄▄▖▝▚▄▞▘

 */

static inline void
scene_selection_seo_transform_core(SceneEditorObjectMesh *, SceneEditorObject *,
                                   vec3 *, seo_transform_axis_callback,
                                   SceneSelectionTransform *);

void scene_selection_seo_transform_core(
    SceneEditorObjectMesh *mesh, SceneEditorObject *seo, vec3 *init_attribute,
    seo_transform_axis_callback transform_callback,
    SceneSelectionTransform *desc) {

  // printf("%s\n", seo->meshes.entries[i].mesh->name);
  //  calculate offset from delta
  vec3 offset_attribute;
  glm_vec3_add(*init_attribute, *desc->delta, offset_attribute);

  // transform seo via their own callback
  transform_callback(&(SEOTransformCallback){
      .mesh = mesh,
      .seo = seo,
      .offset = offset_attribute,
  });

  ssbo_update_queue_insert(&desc->scene->renderer.ssbo, SSBOType_Mesh,
                           mesh->mesh->ssbo_slot.id);
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

    scene_selection_seo_transform_core(&seo->meshes.entries[local_index], seo,
                                       init_attribute, transform_callback,
                                       desc);
  }
}
