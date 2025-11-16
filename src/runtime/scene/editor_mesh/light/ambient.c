#include "./ambient.h"

#include <stddef.h>

#include "backend/registry.h"
#include "backend/resource_manager.h"
#include "backend/theme/core.h"
#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor_mesh/builder/builder.h"
#include "runtime/scene/editor_mesh/list.h"
#include "runtime/systems/scene_editor_mesh_system.h"

/**
   Insert Ambient light gizmo mesh to the list
 */
void sem_ambient_light_create(SceneEditorMeshList *list, AmbientLight *light,
                              const SEMCreateDescriptor *desc) {

  // define mesh
  const size_t gizmo_mesh_count = 1;
  sem_list_create(list, gizmo_mesh_count, "Ambient Light",
                  RegEntryType_SceneEditorMeshList_AmbientLight);

  list->origin = &list->entries[SEM_LIST_ORIGIN_INDEX];

  // get new mesh pointer from main mesh list
  SceneEditorMesh *icon = sem_list_new_entry(list);
  icon->mesh = rem_new_mesh();
  icon->target_list_index = desc->target_list_index;
  icon->target = light;

  const ThemeIconCell *icon_uv =
      theme_icon_cell(&g_theme, ThemeIcon_AmbientLight);

  // create icon mesh
  sem_create_billboard(icon->mesh,
                       &(SEMCreateBillboardDescriptor){
                           .view = theme_icon_atlas(&g_theme),
                           .position = &light->position,
                           .scale = &SEM_BILLBOARD_SCALE,
                           .uv0 = {icon_uv->uv0[0], icon_uv->uv0[1]},
                           .uv1 = {icon_uv->uv1[0], icon_uv->uv1[1]},
                       });

  // set callback
  icon->transform_callback[GizmoMode_Position] =
      sem_system_ambient_light_set_position;
  icon->transform_callback[GizmoMode_Rotation] =
      sem_system_ambient_light_set_rotation;
  icon->transform_callback[GizmoMode_Scale] =
      sem_system_ambient_light_set_scale;
}

// accessor
void sem_list_ambient_light_get_position(SceneEditorMeshList *list,
                                         vec3 value) {
  glm_vec3_copy(((AmbientLight *)list->origin->target)->position, value);
}
void sem_list_ambient_light_get_rotation(SceneEditorMeshList *list,
                                         vec3 value) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, value);
}
void sem_list_ambient_light_get_scale(SceneEditorMeshList *list, vec3 value) {
  glm_vec3_copy((vec3){0.0f, 0.0f, 0.0f}, value);
}
