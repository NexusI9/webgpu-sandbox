#include "reflection_plane.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/scene/editor/object/object.h"
#include "../runtime/scene/scene.h"
#include <stdint.h>

static inline void
seo_probe_reflection_plane_update_mesh_uniform(SceneEditorObject *);

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

  mesh_child_add(probe_cube->mesh, probe_plane->mesh);

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

  mesh_child_add(probe_cube->mesh, probe_arrow->mesh);

  probe_arrow->transform_callback[GizmoMode_Position] =
      seo_probe_reflection_plane_set_position;
  probe_arrow->transform_callback[GizmoMode_Rotation] =
      seo_probe_reflection_plane_set_rotation;
  probe_arrow->transform_callback[GizmoMode_Scale] =
      seo_probe_reflection_plane_set_scale;

  seo->origin = probe_plane->mesh;
}

/**
   Detects which scene meshes are within the probe radius/bound-box and update
   each meshes uniform so subscribe or clear the probes index and count so the
   mesh shader can reference the right probe index for the reflection computing.

   By default the meshes reflection only reflecte the skybox. However if a mesh
   is within a probe reflection bound/radius, it takes the probe ID as to render
   the respective reflection texture in the shader.

   Note that only one reflection plane or grid can be active per mesh.
 */
void seo_probe_reflection_plane_update_mesh_uniform(SceneEditorObject *seo) {

  MeshRefList *pipeline_mesh_list[SCENE_PIPELINE_REFLECTION_COUNT];
  scene_reflection_pipeline_meshes(seo->scene, pipeline_mesh_list);

  SceneEditorObjectMesh *probe_bound_box = &seo->meshes.entries[0];
  ProbeReflectionPlane *probe = (ProbeReflectionPlane *)probe_bound_box->target;
  SSBOManager *ssbo = &seo->scene->renderer.ssbo;

  probe_reflection_plane_update_boundbox(probe);

  for (ScenePipeline i = 0; i < SCENE_PIPELINE_REFLECTION_COUNT; i++) {

    const MeshRefList *pipeline = pipeline_mesh_list[i];

    for (size_t j = 0; j < pipeline->length; j++) {

      Mesh *pipeline_mesh = pipeline->entries[j];
      MeshUniform *uniform = mesh_uniform(pipeline_mesh);
      bool intersect = aabb_intersect(&probe->boundbox,
                                      &pipeline_mesh->topology.boundbox.world);

      if (intersect)
        mesh_uniform_set_probe_reflection_plane(
            pipeline_mesh, probe->ssbo_slot[ProbeReflectionSSBOField_List].id,
            ssbo);
      else
        mesh_uniform_clear_probe_reflection_plane(pipeline_mesh, ssbo);
    }
  }
}

void seo_probe_reflection_plane_set_position(SEOTransformCallback *desc) {

  mesh_set_position(desc->mesh->mesh, desc->offset);

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
  probe_reflection_plane_update_camera(probe);

  // add to upload queue
  ssbo_update_queue_insert(&desc->seo->scene->renderer.ssbo, SSBOType_Camera,
                           probe->ssbo_slot[ProbeReflectionSSBOField_View].id);

  // update scene meshes uniform to define which ones are within the probe area
  // for reflection
  seo_probe_reflection_plane_update_mesh_uniform(desc->seo);
}

void seo_probe_reflection_plane_set_rotation(SEOTransformCallback *desc) {}

void seo_probe_reflection_plane_set_scale(SEOTransformCallback *desc) {

  // print_vec3(desc->offset);
}
