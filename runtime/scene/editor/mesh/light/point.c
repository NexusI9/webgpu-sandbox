#include "./point.h"

#include <cglm/vec3.h>
#include <stddef.h>
#include <stdint.h>

#include "backend/registry.h"
#include "backend/ssbo.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/light/shadow_map/draw.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/debug/core.h"
#include "runtime/scene/editor/mesh/builder/billboard.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/renderer/core.h"
#include "utils/projection.h"

static inline void sem_point_light_create_common(SceneEditorMeshList *,
                                                 PointLight *,
                                                 const SEMCreateDescriptor *,
                                                 const LightCreateFlag);

void sem_point_light_create_common(SceneEditorMeshList *list, PointLight *light,
                                   const SEMCreateDescriptor *desc,
                                   const LightCreateFlag flag) {

  // define mesh
  const RegEntryType type =
      (flag & LightCreateFlag_Shadow)
          ? RegEntryType_SceneEditorMeshList_PointLightShadow
          : RegEntryType_SceneEditorMeshList_PointLight;

  const size_t gizmo_mesh_count = 1;
  sem_list_create(list, gizmo_mesh_count, "Point Light", type);

  list->origin = &list->entries[SEM_LIST_ORIGIN_INDEX];

  // get new mesh pointer from main mesh list
  SceneEditorMesh *icon = sem_list_new_entry(list);
  icon->mesh = scene_new_mesh(desc->scene);
  icon->target = light;
  icon->target_list_index = desc->target_list_index;
  icon->scene = desc->scene;

  SceneEditorUIIconUV icon_uv =
      desc->scene->editor.ui.icon_uv[SceneEditorUIIcon_PointLight];

  // create icon mesh
  sem_create_billboard(icon->mesh,
                       &(SEMCreateBillboardDescriptor){
                           .view = desc->scene->editor.ui.atlas_texture.view,
                           .position = &light->position,
                           .scale = &SEM_BILLBOARD_SCALE,
                           .uv0 = {icon_uv.uv0[0], icon_uv.uv0[1]},
                           .uv1 = {icon_uv.uv1[0], icon_uv.uv1[1]},
                       });
}

// accessor
void sem_list_point_light_get_position(SceneEditorMeshList *list, vec3 value) {
  glm_vec3_copy(((PointLight *)list->origin->target)->position, value);
}
void sem_list_point_light_get_rotation(SceneEditorMeshList *list, vec3 value) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, value);
}
void sem_list_point_light_get_scale(SceneEditorMeshList *list, vec3 value) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, value);
}

/* TODO: Create a more generic sem transform callback*/
void sem_list_point_light_set_position(SceneEditorMeshList *list, vec3 value) {
  for (size_t i = 0; i < list->length; i++) {
    SceneEditorMesh *sem = &list->entries[i];
    sem->transform_callback[GizmoMode_Position](sem, value);
  }
}
void sem_list_point_light_set_rotation(SceneEditorMeshList *sem, vec3 value) {}
void sem_list_point_light_set_scale(SceneEditorMeshList *sem, vec3 value) {}

/**
   Insert Point light gizmo mesh to the list
 */
void sem_point_light_create(SceneEditorMeshList *list, PointLight *light,
                            const SEMCreateDescriptor *desc) {

  sem_point_light_create_common(list, light, desc, LightCreateFlag_None);
  sem_point_light_update_transform_callback(list, LightCreateFlag_None);
}

void sem_point_light_set_position(SceneEditorMesh *sem, vec3 value) {

  PointLight *light = (PointLight *)sem->target;

  glm_vec3_copy(value, light->position);

  point_light_uniform_update(light);
  ssbo_update_queue_insert(&sem->scene->renderer.ssbo, SSBOType_PointLight,
                           light->ssbo_slot[LightSSBOSlot_List].id);

  mesh_set_position(sem->mesh, value);
}

void sem_point_light_set_rotation(SceneEditorMesh *sem, vec3 value) {}
void sem_point_light_set_scale(SceneEditorMesh *sem, vec3 value) {}

/**

    ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▄  ▗▄▖ ▗▖ ▗▖
   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌ ▐▌▐▌ ▐▌
    ▝▀▚▖▐▛▀▜▌▐▛▀▜▌▐▌  █▐▌ ▐▌▐▌ ▐▌
   ▗▄▄▞▘▐▌ ▐▌▐▌ ▐▌▐▙▄▄▀▝▚▄▞▘▐▙█▟▌

 */

static inline void sem_point_light_update_shadow(SceneEditorMesh *);
void sem_point_light_update_shadow(SceneEditorMesh *sem) {

  PointLight *light = (PointLight *)sem->target;
  SSBOManager *ssbo = &sem->scene->renderer.ssbo;

  if (scene_renderer_draw_mode(&sem->scene->renderer) ==
      SceneRendererDrawMode_Texture) {

    // update light views properties (CPU) + update SSBO entries
    point_light_projection_update(light);

    // add to write queue (CPU > GPU)
    for (uint8_t i = 0; i < PROJECTION_VIEW_COUNT; i++)
      ssbo_update_queue_insert(ssbo, SSBOType_ViewProjection,
                               light->ssbo_slot[LightSSBOSlot_View + i].id);

    shadow_map_draw_point_light(
        &(ShadowMapDrawPointLightDescriptor){
            .light = light,
            .pass = &sem->scene->lights.point.shadow.pass,
            .texture_layer = sem->target_list_index,
            .command_encoder = NULL,
            .profiler = &sem->scene->renderer.profiler,
        },
        SCENE_DEBUG_UNDEFINED);
  }
}

void sem_point_light_shadow_create(SceneEditorMeshList *list, PointLight *light,
                                   const SEMCreateDescriptor *desc) {

  sem_point_light_create_common(list, light, desc, LightCreateFlag_Shadow);
  sem_point_light_update_transform_callback(list, LightCreateFlag_Shadow);
}

void sem_point_light_shadow_set_position(SceneEditorMesh *sem, vec3 value) {

  PointLight *light = (PointLight *)sem->target;
  SSBOManager *ssbo = &sem->scene->renderer.ssbo;

  glm_vec3_copy(value, light->position);

  point_light_uniform_update(light);
  ssbo_update_queue_insert(ssbo, SSBOType_PointLight,
                           light->ssbo_slot[LightSSBOSlot_List].id);

  mesh_set_position(sem->mesh, value);
  sem_point_light_update_shadow(sem);
}

void sem_point_light_shadow_set_rotation(SceneEditorMesh *list, vec3 value) {}
void sem_point_light_shadow_set_scale(SceneEditorMesh *list, vec3 value) {}
void sem_list_point_light_shadow_set_position(SceneEditorMeshList *list,
                                              vec3 value) {
  for (size_t i = 0; i < list->length; i++) {
    SceneEditorMesh *sem = &list->entries[i];
    sem->transform_callback[GizmoMode_Position](sem, value);
  }
}
void sem_list_point_light_shadow_set_rotation(SceneEditorMeshList *sem,
                                              vec3 value) {}
void sem_list_point_light_shadow_set_scale(SceneEditorMeshList *sem,
                                           vec3 value) {}

static const sem_transform_axis_callback
    light_transform_callback[][GIZMO_MODE_COUNT] = {
        [LightCreateFlag_None] =
            {
                [GizmoMode_Position] = sem_point_light_set_position,
                [GizmoMode_Rotation] = sem_point_light_set_rotation,
                [GizmoMode_Scale] = sem_point_light_set_scale,
            },
        [LightCreateFlag_Shadow] =
            {
                [GizmoMode_Position] = sem_point_light_shadow_set_position,
                [GizmoMode_Rotation] = sem_point_light_set_rotation,
                [GizmoMode_Scale] = sem_point_light_set_scale,
            },
};

void sem_point_light_update_transform_callback(SceneEditorMeshList *list,
                                               const LightCreateFlag flag) {

  for (size_t i = 0; i < list->length; i++)
    for (GizmoMode j = 0; j < GIZMO_MODE_COUNT; j++)
      list->entries[i].transform_callback[j] =
          light_transform_callback[flag][i];
}
