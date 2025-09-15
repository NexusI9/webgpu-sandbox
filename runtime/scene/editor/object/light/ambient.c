#include "./ambient.h"

#include <stddef.h>

#include "runtime/mesh/transform.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/object/builder/billboard.h"
#include "runtime/scene/editor/object/list/list.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/light/core.h"

/**
   Insert Ambient light gizmo mesh to the list
 */
void seo_light_ambient_create(SceneEditorObject *seo, AmbientLight *light,
                              const SEOCreateDescriptor *desc) {

  // define target

  seo->scene = desc->scene;

  // define mesh
  const size_t gizmo_mesh_count = 1;
  seo_mesh_list_create(&seo->meshes, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  SceneEditorObjectMesh *icon = seo_mesh_list_new_entry(&seo->meshes);
  icon->mesh = scene_new_mesh(desc->scene);
  icon->target_list_index = desc->target_list_index;
  icon->target = light;

  const char *texture_path = "./resources/assets/texture/ui/light-ambient.png";

  // create gizmo mesh
  seo_create_billboard(icon->mesh, &(SEOCreateBillboardDescriptor){
                                       .texture_path = texture_path,
                                       .device = desc->device,
                                       .queue = desc->queue,
                                       .position = &light->position,
                                       .scale = &SEO_BILLBOARD_SCALE,
                                   });

  // set callback
  icon->transform_callback[GizmoMode_Position] =
      seo_light_ambient_set_position;
  icon->transform_callback[GizmoMode_Rotation] =
      seo_light_ambient_set_rotation;
  icon->transform_callback[GizmoMode_Scale] =
      seo_light_ambient_set_scale;

  seo->origin = icon->mesh;
}

void seo_light_ambient_set_position(SEOTransformCallback *desc) {

  for (size_t i = 0; i < desc->seo->meshes.length; i++)
    mesh_set_position(desc->seo->meshes.entries[i].mesh, desc->offset);
}

void seo_light_ambient_set_rotation(SEOTransformCallback *desc) {}

void seo_light_ambient_set_scale(SEOTransformCallback *desc) {}
