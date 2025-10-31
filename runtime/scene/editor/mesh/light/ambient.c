#include "./ambient.h"

#include <stddef.h>

#include "backend/registry.h"
#include "backend/resource_manager.h"
#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/mesh/builder/billboard.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/editor/ui/core.h"

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
  icon->scene = desc->scene;

  SceneEditorUIIconUV icon_uv =
      desc->scene->editor.ui.icon_uv[SceneEditorUIIcon_AmbientLight];

  // create icon mesh
  sem_create_billboard(icon->mesh,
                       &(SEMCreateBillboardDescriptor){
                           .view = desc->scene->editor.ui.atlas_texture.view,
                           .position = &light->position,
                           .scale = &SEM_BILLBOARD_SCALE,
                           .uv0 = {icon_uv.uv0[0], icon_uv.uv0[1]},
                           .uv1 = {icon_uv.uv1[0], icon_uv.uv1[1]},
                       });

  // set callback
  icon->transform_callback[GizmoMode_Position] = sem_ambient_light_set_position;
  icon->transform_callback[GizmoMode_Rotation] = sem_ambient_light_set_rotation;
  icon->transform_callback[GizmoMode_Scale] = sem_ambient_light_set_scale;
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

void sem_list_ambient_light_set_position(SceneEditorMeshList *list,
                                         vec3 value) {
  for (size_t i = 0; i < list->length; i++) {
    SceneEditorMesh *sem = &list->entries[i];
    sem->transform_callback[GizmoMode_Position](sem, value);
  }
}

void sem_list_ambient_light_set_rotation(SceneEditorMeshList *sem, vec3 value) {

}

void sem_list_ambient_light_set_scale(SceneEditorMeshList *sem, vec3 value) {}

// mutator
void sem_ambient_light_set_position(SceneEditorMesh *sem, vec3 value) {
  mesh_set_position(sem->mesh, value);
}
void sem_ambient_light_set_rotation(SceneEditorMesh *sem, vec3 value) {}
void sem_ambient_light_set_scale(SceneEditorMesh *sem, vec3 value) {}
