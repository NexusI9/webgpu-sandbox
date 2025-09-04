#include "reflection_plane.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/scene/editor/object/object.h"
#include "../runtime/scene/scene.h"
#include <stdint.h>

void seo_probe_reflection_plane_create(SceneEditorObject *seo,
                                       ProbeReflectionPlane *probe,
                                       const SEOCreateDescriptor *desc) {

  seo->scene = desc->scene;

  const uint16_t seo_mesh_count = 4;
  seo_mesh_list_create(&seo->meshes, seo_mesh_count);

  /*


     === Distance Cube ===


   */
  SceneEditorObjectMesh *probe_cube = seo_mesh_list_new_entry(&seo->meshes);
  probe_cube->mesh = scene_new_mesh(desc->scene);
  probe_cube->target = probe;
  probe_cube->target_list_index = SCENE_EDITOR_OBJECT_TARGET_UNDEFINED;

  Primitive primitive_cube;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/cube.mbin",
      .primitive = &primitive_cube,
  });

  SEOCreateWireframeDescriptor wireframe_cube_desc = {
      .color = &(color){1.0f, 0.0f, 0.0f, 1.0f},
      .device = desc->device,
      .queue = desc->queue,
      .index = &primitive_cube.index,
      .vertex = &primitive_cube.vertex,
      .name = "seo probe reflection plane",
      .thickness = SEO_WIREFRAME_LINE_THICKNESS,
  };

  seo_create_wireframe(probe_cube->mesh, &wireframe_cube_desc);

  mesh_set_scale(probe_cube->mesh, (vec3){
                                       probe->scale[0],
                                       probe->distance,
                                       probe->scale[2],
                                   });
  mesh_set_position(probe_cube->mesh, probe->position);

  probe_cube->transform_callback[GizmoMode_Position] =
      seo_probe_reflection_plane_set_position;
  probe_cube->transform_callback[GizmoMode_Rotation] =
      seo_probe_reflection_plane_set_rotation;
  probe_cube->transform_callback[GizmoMode_Scale] =
      seo_probe_reflection_plane_set_scale;

  /*


    === Main refletion plane ===


   */
  SceneEditorObjectMesh *probe_plane = seo_mesh_list_new_entry(&seo->meshes);

  probe_plane->mesh = scene_new_mesh(desc->scene);
  probe_plane->target = probe;
  probe_plane->target_list_index =
      SCENE_EDITOR_OBJECT_TARGET_UNDEFINED; // necessary ?

  Primitive primitive;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/plane.mbin",
      .primitive = &primitive,
  });

  SEOCreateWireframeDescriptor wireframe_desc = {
      .color = &(color){1.0f, 0.0f, 0.0f, 1.0f},
      .device = desc->device,
      .queue = desc->queue,
      .index = &primitive.index,
      .vertex = &primitive.vertex,
      .name = "seo probe reflection plane",
      .thickness = SEO_WIREFRAME_LINE_THICKNESS,
  };

  seo_create_wireframe(probe_plane->mesh, &wireframe_desc);

  mesh_set_scale(probe_plane->mesh, probe->scale);
  mesh_set_position(probe_plane->mesh, probe->position);

  probe_plane->transform_callback[GizmoMode_Position] =
      seo_probe_reflection_plane_set_position;
  probe_plane->transform_callback[GizmoMode_Rotation] =
      seo_probe_reflection_plane_set_rotation;
  probe_plane->transform_callback[GizmoMode_Scale] =
      seo_probe_reflection_plane_set_scale;

  /*


   === Normal Arrow ===


 */
  SceneEditorObjectMesh *probe_arrow = seo_mesh_list_new_entry(&seo->meshes);
  probe_arrow->mesh = scene_new_mesh(desc->scene);
  probe_arrow->target = probe;
  probe_arrow->target_list_index = SCENE_EDITOR_OBJECT_TARGET_UNDEFINED;

  Primitive primitive_arrow;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/arrow.mbin",
      .primitive = &primitive_arrow,
  });

  SEOCreateWireframeDescriptor wireframe_arrow_desc = {
      .color = &(color){1.0f, 0.0f, 0.0f, 1.0f},
      .device = desc->device,
      .queue = desc->queue,
      .index = &primitive_arrow.index,
      .vertex = &primitive_arrow.vertex,
      .name = "seo probe reflection plane",
      .thickness = SEO_WIREFRAME_LINE_THICKNESS,
  };

  seo_create_wireframe(probe_arrow->mesh, &wireframe_arrow_desc);
  mesh_set_position(probe_arrow->mesh, probe->position);

  probe_arrow->transform_callback[GizmoMode_Position] =
      seo_probe_reflection_plane_set_position;
  probe_arrow->transform_callback[GizmoMode_Rotation] =
      seo_probe_reflection_plane_set_rotation;
  probe_arrow->transform_callback[GizmoMode_Scale] =
      seo_probe_reflection_plane_set_scale;

  seo->origin = probe_plane->mesh;
}

void seo_probe_reflection_plane_set_position(SEOTransformCallback *desc) {

  for (size_t i = 0; i < desc->seo->meshes.length; i++)
    mesh_set_position(desc->seo->meshes.entries[i].mesh, desc->offset);

  ProbeReflectionPlane *probe = (ProbeReflectionPlane *)desc->mesh->target;
  glm_vec3_copy(desc->mesh->mesh->position, probe->position);
  probe->signed_distance = glm_dot(probe->normal, probe->position);

  // update uniform cpu side
  probe_reflection_plane_update_uniform(probe);

  // add to upload queue
  ssbo_update_queue_insert(&desc->seo->scene->renderer.ssbo,
                           SSBOType_ProbePlaneReflection,
                           probe->ssbo_slot[ProbeReflectionSSBOField_List].id);

  // update view cpu side
  probe_reflection_plane_update_view(probe);

  // add to upload queue
  ssbo_update_queue_insert(&desc->seo->scene->renderer.ssbo,
                           SSBOType_ViewProjection,
                           probe->ssbo_slot[ProbeReflectionSSBOField_View].id);
}

void seo_probe_reflection_plane_set_rotation(SEOTransformCallback *desc) {}

void seo_probe_reflection_plane_set_scale(SEOTransformCallback *desc) {

  // print_vec3(desc->offset);
}
