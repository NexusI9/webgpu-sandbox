#include "./point.h"

#include <cglm/vec3.h>
#include <stddef.h>
#include <stdint.h>

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
#include "runtime/scene/renderer/core.h"
#include "utils/projection.h"

static inline void sem_light_point_create_common(SceneEditorMeshList *,
                                                 PointLight *,
                                                 const SEMCreateDescriptor *);

void sem_light_point_create_common(SceneEditorMeshList *list, PointLight *light,
                                   const SEMCreateDescriptor *desc) {

  // define mesh
  const size_t gizmo_mesh_count = 1;
  sem_list_create(list, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  SceneEditorMesh *icon = sem_list_new_entry(list);
  icon->mesh = scene_new_mesh(desc->scene);
  icon->target = light;
  icon->target_list_index = desc->target_list_index;
  icon->scene = desc->scene;

  const char *texture_path = "./resources/assets/texture/ui/light-point.png";

  // create gizmo mesh
  sem_create_billboard(icon->mesh, &(SEMCreateBillboardDescriptor){
                                       .texture_path = texture_path,
                                       .position = &light->position,
                                       .scale = &SEM_BILLBOARD_SCALE,
                                   });
}

/**
   Insert Point light gizmo mesh to the list
 */
void sem_light_point_create(SceneEditorMeshList *list, PointLight *light,
                            const SEMCreateDescriptor *desc) {

  sem_light_point_create_common(list, light, desc);
  sem_light_point_update_transform_callback(list, LightShadow_None);
}

void sem_light_point_set_position(SEMTransformCallback *desc) {

  PointLight *light = (PointLight *)desc->sem->target;

  glm_vec3_copy(desc->offset, light->position);

  light_point_uniform_update(light);
  ssbo_update_queue_insert(&desc->sem->scene->renderer.ssbo,
                           SSBOType_PointLight,
                           light->ssbo_slot[LightSSBOSlot_List].id);

  mesh_set_position(desc->sem->mesh, desc->offset);
}

void sem_light_point_set_rotation(SEMTransformCallback *desc) {}

void sem_light_point_set_scale(SEMTransformCallback *desc) {}

/**

    ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▄  ▗▄▖ ▗▖ ▗▖
   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌  █▐▌ ▐▌▐▌ ▐▌
    ▝▀▚▖▐▛▀▜▌▐▛▀▜▌▐▌  █▐▌ ▐▌▐▌ ▐▌
   ▗▄▄▞▘▐▌ ▐▌▐▌ ▐▌▐▙▄▄▀▝▚▄▞▘▐▙█▟▌

 */
void sem_light_point_shadow_create(SceneEditorMeshList *list, PointLight *light,
                                   const SEMCreateDescriptor *desc) {

  sem_light_point_create_common(list, light, desc);
  sem_light_point_update_transform_callback(list, LightShadow_Enabled);
}

void sem_light_point_shadow_set_position(SEMTransformCallback *desc) {

  PointLight *light = (PointLight *)desc->sem->target;
  SSBOManager *ssbo = &desc->sem->scene->renderer.ssbo;

  glm_vec3_copy(desc->offset, light->position);

  light_point_uniform_update(light);
  ssbo_update_queue_insert(ssbo, SSBOType_PointLight,
                           light->ssbo_slot[LightSSBOSlot_List].id);

  mesh_set_position(desc->sem->mesh, desc->offset);

  // update light shadow map
  if (scene_renderer_draw_mode(&desc->sem->scene->renderer) ==
      SceneRendererDrawMode_Texture) {

    // update light views properties (CPU) + update SSBO entries
    light_point_projection_update(light);

    // add to write queue (CPU > GPU)
    for (uint8_t i = 0; i < PROJECTION_VIEW_COUNT; i++)
      ssbo_update_queue_insert(ssbo, SSBOType_ViewProjection,
                               light->ssbo_slot[LightSSBOSlot_View + i].id);

    shadow_map_draw_point_light(
        &(ShadowMapDrawPointLightDescriptor){
            .light = light,
            .pass = &desc->sem->scene->lights.point.shadow.pass,
            .texture_layer = desc->sem->target_list_index,
            .command_encoder = NULL,
        },
        SCENE_DEBUG_UNDEFINED);
  }
}

static const sem_transform_axis_callback
    light_transform_callback[2][GIZMO_MODE_COUNT] = {
        [LightShadow_None] =
            {
                [GizmoMode_Position] = sem_light_point_set_position,
                [GizmoMode_Rotation] = sem_light_point_set_rotation,
                [GizmoMode_Scale] = sem_light_point_set_scale,
            },
        [LightShadow_Enabled] =
            {
                [GizmoMode_Position] = sem_light_point_shadow_set_position,
                [GizmoMode_Rotation] = sem_light_point_set_rotation,
                [GizmoMode_Scale] = sem_light_point_set_scale,
            },
};

void sem_light_point_update_transform_callback(SceneEditorMeshList *list,
                                               const LightShadow shadow) {

  for (size_t i = 0; i < list->length; i++)
    for (GizmoMode j = 0; j < GIZMO_MODE_COUNT; j++)
      list->entries[i].transform_callback[j] =
          light_transform_callback[shadow][i];
}
