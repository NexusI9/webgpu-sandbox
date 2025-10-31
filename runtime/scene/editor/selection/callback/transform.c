#include "transform.h"

#include <cglm/types.h>
#include <cglm/vec3.h>
#include <stdbool.h>
#include <stddef.h>

#include "backend/logger.h"
#include "backend/registry.h"
#include "backend/ubo.h"
#include "runtime/geometry/aabb/aabb.h"
#include "runtime/light/shadow_map/draw.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/mesh/uniform.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/core.h"
#include "runtime/scene/debug/core.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/renderer/core.h"

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
    ProbeReflectionPlaneList *plane_list, UBOManager *ubo) {
  size_t i = 0;

  MeshUniform *uniform = mesh_uniform(mesh);

  for (i = 0; i < plane_list->length; i++) {

    ProbeReflectionPlane *probe = &plane_list->entries[i];
    bool intersect =
        aabb_intersect(&mesh->topology.boundbox.world, &probe->boundbox);

    if (intersect)
      mesh_uniform_set_probe_reflection_plane(mesh, ubo);
    else
      mesh_uniform_clear_probe_reflection_plane(mesh, ubo);
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

  mesh_uniform_update(mesh);
  ubo_update_queue_insert(&desc->scene->renderer.ubo, UBOType_Mesh,
                          mesh->ubo_slot.id);
}

/* Mesh based transform */
void scene_selection_mesh_transform(SceneSelectionTransform *desc) {

  for (size_t i = 0; i < desc->selection->length; i++) {
    Mesh *mesh = desc->selection->entries[i].mesh;
    vec3 *init_attribute = &desc->selection->entries[i].initial_attribute;

    // transform mesh
    scene_selection_mesh_transform_core(mesh, init_attribute, desc);
  }
}

/* Mesh shadow based transform.
   Note that this filter only incudes meshes that are in the LitShadow pipelines
 */
void scene_selection_mesh_shadow_transform(SceneSelectionTransform *desc) {

  for (size_t i = 0; i < desc->selection->length; i++) {
    Mesh *mesh = desc->selection->entries[i].mesh;
    vec3 *init_attribute = &desc->selection->entries[i].initial_attribute;
    // transform mesh
    scene_selection_mesh_transform_core(mesh, init_attribute, desc);
  }

  // recalculate shadow maps
  if (desc->scene->renderer.draw.mode == SceneRendererDrawMode_Texture)
    shadow_map_draw_all(
        &(ShadowMapDrawAllDescriptor){
            .mesh_list =
                scene_pipeline(desc->scene, ScenePipeline_Dynamic_LitShadow),
            .lights = &desc->scene->lights,
            .profiler = &desc->scene->renderer.profiler,
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
scene_selection_sem_transform_core(SceneEditorMesh *, vec3 *,
                                   sem_transform_axis_callback,
                                   SceneSelectionTransform *);

void scene_selection_sem_transform_core(
    SceneEditorMesh *sem, vec3 *init_attribute,
    sem_transform_axis_callback transform_callback,
    SceneSelectionTransform *desc) {

  //  calculate offset from delta
  vec3 offset_attribute;
  glm_vec3_add(*init_attribute, *desc->delta, offset_attribute);

  // transform sem via their own callback
  transform_callback(sem, offset_attribute);
  mesh_uniform_update(sem->mesh);
  ubo_update_queue_insert(&desc->scene->renderer.ubo, UBOType_Mesh,
                          sem->mesh->ubo_slot.id);
}

/*
  SEM based transform

   .--------------------------------------------------------------.
   |                       Actives Meshes                         |
   |------------------------------.-------------------------------|
   | Mesh 1 |  Mesh 2  |  Mesh 3  |  Mesh 1  |  Mesh 2  | Mesh 3  |
   |------------------------------+-------------------------------|
   |           SEM 1              |            SEM 2              |
   '------------------------------'-------------------------------'

 */
void scene_selection_sem_transform(SceneSelectionTransform *desc) {

  size_t offset = 0;

  for (size_t i = 0; i < desc->selection->length; i++) {

    vec3 *init_attribute = &desc->selection->entries[i].initial_attribute;
    Mesh *mesh = desc->selection->entries[i].mesh;

    const RegEntry *reg_entry = reg_lookup(desc->selection->entries[i].target);
    SceneEditorMeshList *sem_list = (SceneEditorMeshList *)reg_entry->ptr;

    for (size_t i = 0; i < sem_list->length; i++) {
      SceneEditorMesh *sem = &sem_list->entries[i];
      scene_selection_sem_transform_core(
          sem, init_attribute, sem->transform_callback[desc->transform_mode],
          desc);
    }
  }
}
