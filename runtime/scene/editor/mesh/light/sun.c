#include "sun.h"

#include <cglm/vec3.h>
#include <stddef.h>

#include "backend/registry.h"
#include "backend/resource_manager.h"
#include "backend/theme/core.h"
#include "backend/ubo.h"
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
#include "runtime/scene/renderer/core.h"

static inline void sem_sun_light_create_common(SceneEditorMeshList *,
                                               SunLight *,
                                               const SEMCreateDescriptor *,
                                               const LightCreateFlag flag);

void sem_sun_light_create_common(SceneEditorMeshList *list, SunLight *light,
                                 const SEMCreateDescriptor *desc,
                                 const LightCreateFlag flag) {

  // define mesh
  const RegEntryType type =
      (flag & LightCreateFlag_Shadow)
          ? RegEntryType_SceneEditorMeshList_SunLightShadow
          : RegEntryType_SceneEditorMeshList_SunLight;

  const size_t gizmo_mesh_count = 1;
  sem_list_create(list, gizmo_mesh_count, "Sun Light", type);

  list->origin = &list->entries[SEM_LIST_ORIGIN_INDEX];

  // get new mesh pointer from main mesh list
  SceneEditorMesh *icon = sem_list_new_entry(list);
  icon->mesh = rem_new_mesh();
  icon->target = light;
  icon->target_list_index = desc->target_list_index;
  icon->scene = desc->scene;

  const ThemeIconCell *icon_uv = theme_icon_cell(&g_theme, ThemeIcon_SunLight);

  // create icon mesh
  sem_create_billboard(icon->mesh,
                       &(SEMCreateBillboardDescriptor){
                           .view = theme_icon_atlas(&g_theme),
                           .position = &light->position,
                           .scale = &SEM_BILLBOARD_SCALE,
                           .uv0 = {icon_uv->uv0[0], icon_uv->uv0[1]},
                           .uv1 = {icon_uv->uv1[0], icon_uv->uv1[1]},
                       });

}

/**
   Insert Sun light gizmo mesh to the list
 */
void sem_sun_light_create(SceneEditorMeshList *list, SunLight *light,
                          const SEMCreateDescriptor *desc) {
  sem_sun_light_create_common(list, light, desc, LightCreateFlag_None);
  sem_sun_light_update_transform_callback(list, LightCreateFlag_None);
}

// accessor
void sem_list_sun_light_get_position(SceneEditorMeshList *sem, vec3 value) {
  glm_vec3_copy(((SunLight *)sem->origin->target)->position, value);
}
void sem_list_sun_light_get_rotation(SceneEditorMeshList *sem, vec3 value) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, value);
}
void sem_list_sun_light_get_scale(SceneEditorMeshList *sem, vec3 value) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, value);
}

void sem_list_sun_light_set_position(SceneEditorMeshList *list, vec3 value) {
  for (size_t i = 0; i < list->length; i++) {
    SceneEditorMesh *sem = &list->entries[i];
    sem->transform_callback[GizmoMode_Position](sem, value);
  }
}
void sem_list_sun_light_set_rotation(SceneEditorMeshList *list, vec3 value) {}
void sem_list_sun_light_set_scale(SceneEditorMeshList *list, vec3 value) {}

void sem_sun_light_set_position(SceneEditorMesh *sem, vec3 value) {

  SunLight *light = (SunLight *)sem->target;

  glm_vec3_copy(value, light->position);

  sun_light_uniform_update(light);

  ubo_update_queue_insert(&sem->scene->renderer.ubo, UBOType_LightList,
                          sem->scene->lights.ubo_slot.id);

  mesh_set_position(sem->mesh, value);
}

void sem_sun_light_set_rotation(SceneEditorMesh *sem, vec3 value) {}
void sem_sun_light_set_scale(SceneEditorMesh *sem, vec3 value) {}

/**

    ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▄  ▗▄▖ ▗▖ ▗▖
   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌ ▐▌▐▌ ▐▌
    ▝▀▚▖▐▛▀▜▌▐▛▀▜▌▐▌  █▐▌ ▐▌▐▌ ▐▌
   ▗▄▄▞▘▐▌ ▐▌▐▌ ▐▌▐▙▄▄▀▝▚▄▞▘▐▙█▟▌

 */

static inline void sem_sun_light_update_shadow(SceneEditorMesh *);

void sem_spot_light_update_shadow(SceneEditorMesh *sem) {

  SunLight *light = (SunLight *)sem->target;
  UBOManager *ubo = &sem->scene->renderer.ubo;

  if (scene_renderer_draw_mode(&sem->scene->renderer) ==
      SceneRendererDrawMode_Texture) {

    sun_light_projection_update(light);

    ubo_update_queue_insert(ubo, UBOType_ViewProjection,
                            light->ubo_projection.id);

    shadow_map_draw_sun_light(
        &(ShadowMapDrawSunLightDescriptor){
            .light = light,
            .pass = &sem->scene->lights.spot.shadow.pass,
            .texture_layer =
                sem->scene->lights.spot.shadow.length + sem->target_list_index,
            .command_encoder = NULL,
            .profiler = &sem->scene->renderer.profiler,
        },
        SCENE_DEBUG_UNDEFINED);
  }
}

/**
   Insert Shadowed Sun light gizmo mesh to the list
 */
void sem_sun_light_shadow_create(SceneEditorMeshList *list, SunLight *light,
                                 const SEMCreateDescriptor *desc) {

  sem_sun_light_create_common(list, light, desc, LightCreateFlag_Shadow);
  sem_sun_light_update_transform_callback(list, LightCreateFlag_Shadow);
}

void sem_sun_light_shadow_set_position(SceneEditorMesh *sem, vec3 value) {

  SunLight *light = (SunLight *)sem->target;
  UBOManager *ubo = &sem->scene->renderer.ubo;

  glm_vec3_copy(value, light->position);

  sun_light_uniform_update(light);
  ubo_update_queue_insert(&sem->scene->renderer.ubo, UBOType_LightList,
                          sem->scene->lights.ubo_slot.id);

  mesh_set_position(sem->mesh, value);
  sem_spot_light_update_shadow(sem);
}

void sem_sun_light_shadow_set_rotation(SceneEditorMesh *sem, vec3 value) {}
void sem_sun_light_shadow_set_scale(SceneEditorMesh *sem, vec3 value) {}

void sem_list_sun_light_shadow_set_position(SceneEditorMeshList *list,
                                            vec3 value) {
  for (size_t i = 0; i < list->length; i++) {
    SceneEditorMesh *sem = &list->entries[i];
    sem->transform_callback[GizmoMode_Position](sem, value);
  }
}
void sem_list_sun_light_shadow_set_rotation(SceneEditorMeshList *list,
                                            vec3 value) {}
void sem_list_sun_light_shadow_set_scale(SceneEditorMeshList *list,
                                         vec3 value) {}

static const sem_transform_axis_callback
    light_transform_callback[][GIZMO_MODE_COUNT] = {
        [LightCreateFlag_None] =
            {
                [GizmoMode_Position] = sem_sun_light_set_position,
                [GizmoMode_Rotation] = sem_sun_light_set_rotation,
                [GizmoMode_Scale] = sem_sun_light_set_scale,
            },
        [LightCreateFlag_Shadow] =
            {
                [GizmoMode_Position] = sem_sun_light_shadow_set_position,
                [GizmoMode_Rotation] = sem_sun_light_set_rotation,
                [GizmoMode_Scale] = sem_sun_light_set_scale,
            },
};

void sem_sun_light_update_transform_callback(SceneEditorMeshList *list,
                                             const LightCreateFlag flag) {

  for (size_t i = 0; i < list->length; i++)
    for (GizmoMode j = 0; j < GIZMO_MODE_COUNT; j++)
      list->entries[i].transform_callback[j] =
          light_transform_callback[flag][i];
}
