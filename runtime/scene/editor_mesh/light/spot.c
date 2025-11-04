#include "spot.h"

#include <cglm/vec3.h>
#include <stddef.h>

#include "backend/registry.h"
#include "backend/renderer/core.h"
#include "backend/resource_manager.h"
#include "backend/theme/core.h"
#include "backend/ubo.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/debug/core.h"
#include "runtime/scene/editor_mesh/builder/builder.h"
#include "runtime/scene/editor_mesh/list.h"
#include "runtime/systems/scene_editor_mesh_system.h"

static inline void
sem_spot_light_update_transform_callback(SceneEditorMeshList *,
                                         const LightCreateFlag);

static inline void sem_spot_light_create_common(SceneEditorMeshList *,
                                                SpotLight *,
                                                const SEMCreateDescriptor *,
                                                const LightCreateFlag);

void sem_spot_light_create_common(SceneEditorMeshList *list, SpotLight *light,
                                  const SEMCreateDescriptor *desc,
                                  const LightCreateFlag flag) {

  // define mesh
  const RegEntryType type =
      (flag & LightCreateFlag_Shadow)
          ? RegEntryType_SceneEditorMeshList_SpotLightShadow
          : RegEntryType_SceneEditorMeshList_SpotLight;

  // define mesh
  const size_t gizmo_mesh_count = 1;
  sem_list_create(list, gizmo_mesh_count, "Spot Light",
                  RegEntryType_SceneEditorMeshList_SpotLight);

  list->origin = &list->entries[SEM_LIST_ORIGIN_INDEX];

  // get new mesh pointer from main mesh list
  SceneEditorMesh *icon = sem_list_new_entry(list);
  icon->mesh = rem_new_mesh();
  icon->target = light;
  icon->target_list_index = desc->target_list_index;

  const ThemeIconCell *icon_uv = theme_icon_cell(&g_theme, ThemeIcon_SpotLight);

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
   Insert Spot light gizmo mesh to the list
 */
void sem_spot_light_create(SceneEditorMeshList *list, SpotLight *light,
                           const SEMCreateDescriptor *desc) {

  sem_spot_light_create_common(list, light, desc, LightCreateFlag_None);
  sem_spot_light_update_transform_callback(list, LightCreateFlag_None);
}

// accessor
void sem_list_spot_light_get_position(SceneEditorMeshList *list, vec3 value) {
  glm_vec3_copy(((SpotLight *)list->origin->target)->position, value);
}
void sem_list_spot_light_get_rotation(SceneEditorMeshList *list, vec3 value) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, value);
}
void sem_list_spot_light_get_scale(SceneEditorMeshList *list, vec3 value) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, value);
}

static const sem_transform_callback
    light_transform_callback[2][GIZMO_MODE_COUNT] = {
        [LightCreateFlag_None] =
            {
                [GizmoMode_Position] = sem_system_spot_light_set_position,
                [GizmoMode_Rotation] = sem_system_spot_light_set_rotation,
                [GizmoMode_Scale] = sem_system_spot_light_set_scale,
            },
        [LightCreateFlag_Shadow] =
            {
                [GizmoMode_Position] =
                    sem_system_spot_light_shadow_set_position,
                [GizmoMode_Rotation] =
                    sem_system_spot_light_shadow_set_rotation,
                [GizmoMode_Scale] = sem_system_spot_light_set_scale,
            },
};

void sem_spot_light_update_transform_callback(SceneEditorMeshList *list,
                                              const LightCreateFlag flag) {

  for (size_t i = 0; i < list->length; i++)
    for (GizmoMode j = 0; j < GIZMO_MODE_COUNT; j++)
      list->entries[i].transform_callback[j] =
          light_transform_callback[flag][i];
}
