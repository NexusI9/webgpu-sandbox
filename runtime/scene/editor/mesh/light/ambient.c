#include "./ambient.h"

#include <stddef.h>

#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/mesh/builder/billboard.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/selection/gizmo/core.h"

/**
   Insert Ambient light gizmo mesh to the list
 */
void sem_light_ambient_create(SceneEditorMeshList *list, AmbientLight *light,
                              const SEMCreateDescriptor *desc) {

  // define mesh
  const size_t gizmo_mesh_count = 1;
  sem_list_create(list, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  SceneEditorMesh *icon = sem_list_new_entry(list);
  icon->mesh = scene_new_mesh(desc->scene);
  icon->target_list_index = desc->target_list_index;
  icon->target = light;
  icon->scene = desc->scene;

  const char *texture_path = "./resources/assets/texture/ui/light-ambient.png";

  // create gizmo mesh
  sem_create_billboard(icon->mesh, &(SEMCreateBillboardDescriptor){
                                       .texture_path = texture_path,
                                       .position = &light->position,
                                       .scale = &SEM_BILLBOARD_SCALE,
                                   });

  // set callback
  icon->transform_callback[GizmoMode_Position] = sem_light_ambient_set_position;
  icon->transform_callback[GizmoMode_Rotation] = sem_light_ambient_set_rotation;
  icon->transform_callback[GizmoMode_Scale] = sem_light_ambient_set_scale;
}

void sem_light_ambient_set_position(SEMTransformCallback *desc) {
  mesh_set_position(desc->sem->mesh, desc->offset);
}

void sem_light_ambient_set_rotation(SEMTransformCallback *desc) {}

void sem_light_ambient_set_scale(SEMTransformCallback *desc) {}
