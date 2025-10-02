#include "reflection_plane.h"

#include <cglm/types.h>
#include <cglm/vec3.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "backend/ssbo.h"
#include "resources/loader/loader.mbin.h"
#include "runtime/geometry/aabb/aabb.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/mesh/uniform.h"
#include "runtime/primitive/core.h"
#include "runtime/probe/reflection/core.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/mesh/builder/wireframe.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/renderer/render_pass/visibility.h"
#include "utils/color.h"

void sem_probe_reflection_plane_create(SceneEditorMeshList *list,
                                       ProbeReflectionPlane *probe,
                                       const SEMCreateDescriptor *desc) {

  const uint16_t sem_mesh_count = 4;
  sem_list_create(list, sem_mesh_count);

  /*

     === Distance Cube ===

   */
  SceneEditorMesh *probe_cube = sem_list_new_entry(list);
  probe_cube->mesh = scene_new_mesh(desc->scene);

  Primitive primitive_cube;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/cube.mbin",
      .primitive = &primitive_cube,
  });

  SEMCreateWireframeDescriptor wireframe_cube_desc = {
      .color = &(color){0.0f, 0.0f, 0.0f, 1.0f},
      .index = &primitive_cube.index,
      .vertex = &primitive_cube.vertex,
      .name = "sem probe reflection plane",
      .thickness = SEM_WIREFRAME_LINE_THICKNESS,
  };

  sem_create_wireframe(probe_cube->mesh, &wireframe_cube_desc);

  mesh_set_scale(probe_cube->mesh, (vec3){
                                       probe->scale[0],
                                       probe->distance,
                                       probe->scale[2],
                                   });
  mesh_set_position(probe_cube->mesh, probe->position);
  /*

    === Main refletion plane ===

   */
  SceneEditorMesh *probe_plane = sem_list_new_entry(list);
  probe_plane->mesh = scene_new_mesh(desc->scene);

  Primitive primitive;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/plane.mbin",
      .primitive = &primitive,
  });

  SEMCreateWireframeDescriptor wireframe_desc = {
      .color = &(color){0.0f, 0.0f, 0.0f, 1.0f},
      .index = &primitive.index,
      .vertex = &primitive.vertex,
      .name = "sem probe reflection plane",
      .thickness = SEM_WIREFRAME_LINE_THICKNESS,
  };

  sem_create_wireframe(probe_plane->mesh, &wireframe_desc);

  mesh_set_scale(probe_plane->mesh, probe->scale);
  mesh_set_position(probe_plane->mesh, probe->position);

  mesh_child_add(probe_cube->mesh, probe_plane->mesh);

  /*

   === Normal Arrow ===

 */
  SceneEditorMesh *probe_arrow = sem_list_new_entry(list);
  probe_arrow->mesh = scene_new_mesh(desc->scene);

  Primitive primitive_arrow;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/arrow.mbin",
      .primitive = &primitive_arrow,
  });

  SEMCreateWireframeDescriptor wireframe_arrow_desc = {
      .color = &(color){0.0f, 0.0f, 0.0f, 1.0f},
      .index = &primitive_arrow.index,
      .vertex = &primitive_arrow.vertex,
      .name = "sem probe reflection plane",
      .thickness = SEM_WIREFRAME_LINE_THICKNESS,
  };

  sem_create_wireframe(probe_arrow->mesh, &wireframe_arrow_desc);
  mesh_set_position(probe_arrow->mesh, probe->position);

  mesh_child_add(probe_cube->mesh, probe_arrow->mesh);

  // apply sem commons attributes
  for (uint8_t i = 0; i < list->length; i++) {

    list->entries[i].scene = desc->scene;
    list->entries[i].target = probe;
    list->entries[i].target_list_index = SCENE_EDITOR_MESH_TARGET_UNDEFINED;

    list->entries[i].transform_callback[GizmoMode_Position] =
        sem_probe_reflection_plane_set_position;
    list->entries[i].transform_callback[GizmoMode_Rotation] =
        sem_probe_reflection_plane_set_rotation;
    list->entries[i].transform_callback[GizmoMode_Scale] =
        sem_probe_reflection_plane_set_scale;
    
    list->entries[i].select_callback = sem_wireframe_select_callback;
    list->entries[i].deselect_callback = sem_wireframe_deselect_callback;
  }
}

/**
   Detects which scene meshes are within the probe radius/bound-box and update
   each meshes uniform so subscribe or clear the probes index and count so the
   mesh shader can reference the right probe index for the reflection computing.

   By default the meshes reflection only reflecte the skybox. However if a mesh
   is within a probe reflection bound/radius, it takes the probe ID as to render
   the respective reflection texture in the shader.

   Note that only one reflection plane or grid can be active per mesh.

   NOTE:
   Currently this function in unused as automatically assigning reflected meshes
   cause various issue in deciding if a mesh should be self-reflected or not, it
   uselessly complexify the overall process.

   As a solution to this we directly assign/ bind each mesh a probe directly to
   have more efficient and optimized control on the probe reflection handle and
   self reflection.
 */
void sem_probe_reflection_plane_update_mesh_uniform(SceneEditorMesh *sem) {

  MeshRefList *pipeline_mesh_list[SCENE_PIPELINE_REFLECTION_COUNT];
  scene_reflection_pipeline_meshes(sem->scene, pipeline_mesh_list);

  ProbeReflectionPlane *probe = (ProbeReflectionPlane *)sem->target;
  SSBOManager *ssbo = &sem->scene->renderer.ssbo;

  probe_reflection_plane_update_boundbox(probe);

  for (ScenePipeline i = 0; i < SCENE_PIPELINE_REFLECTION_COUNT; i++) {

    const MeshRefList *pipeline = pipeline_mesh_list[i];

    for (size_t j = 0; j < pipeline->length; j++) {

      Mesh *pipeline_mesh = pipeline->entries[j];
      MeshUniform *uniform = mesh_uniform(pipeline_mesh);
      bool intersect = aabb_intersect(&probe->boundbox,
                                      &pipeline_mesh->topology.boundbox.world);

      if (intersect) {
        mesh_uniform_set_probe_reflection_plane(pipeline_mesh, ssbo);
        render_pass_disable_mesh(&sem->scene->planes_reflection.pass,
                                 pipeline_mesh);
      } else {
        mesh_uniform_clear_probe_reflection_plane(pipeline_mesh, ssbo);
        render_pass_enable_mesh(&sem->scene->planes_reflection.pass,
                                pipeline_mesh);
      }
    }
  }
}

void sem_probe_reflection_plane_set_position(SEMTransformCallback *desc) {

  mesh_set_position(desc->sem->mesh, desc->offset);

  ProbeReflectionPlane *probe = (ProbeReflectionPlane *)desc->sem->target;
  glm_vec3_copy(desc->sem->mesh->position, probe->position);
  probe->signed_distance = glm_dot(probe->normal, probe->position);

  // update uniform cpu side
  probe_reflection_plane_update_uniform(probe);

  // add to upload queue
  ssbo_update_queue_insert(&desc->sem->scene->renderer.ssbo,
                           SSBOType_ProbePlaneReflection,
                           probe->ssbo_slot[ProbeReflectionSSBOField_List].id);

  // update view cpu side
  probe_reflection_plane_update_camera(probe);

  // add to upload queue
  ssbo_update_queue_insert(
      &desc->sem->scene->renderer.ssbo, SSBOType_Camera,
      probe->ssbo_slot[ProbeReflectionSSBOField_Camera].id);
}

void sem_probe_reflection_plane_set_rotation(SEMTransformCallback *desc) {}

void sem_probe_reflection_plane_set_scale(SEMTransformCallback *desc) {

  // print_vec3(desc->offset);
}
