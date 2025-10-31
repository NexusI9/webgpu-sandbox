#include "reflection_grid.h"

#include <cglm/types.h>
#include <cglm/vec3.h>
#include <stddef.h>
#include <stdint.h>

#include "backend/logger.h"
#include "backend/registry.h"
#include "backend/resource_manager.h"
#include "backend/ubo.h"
#include "resources/loader/loader.mbin.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/primitive/core.h"
#include "runtime/probe/reflection/core.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/probe.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/mesh/builder/wireframe.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "utils/color.h"

void sem_probe_reflection_grid_create(SceneEditorMeshList *list,
                                      ProbeReflectionGrid *grid,
                                      const SEMCreateDescriptor *desc) {

  // 1 bound cube + (x * y * z probes)
  const uint16_t sem_mesh_count =
      1 + grid->count[0] * grid->count[1] * grid->count[2];
  sem_list_create(list, sem_mesh_count, "Probe Reflection Grid",
                  RegEntryType_SceneEditorMeshList_ProbeReflectionGrid);

  list->origin = &list->entries[SEM_LIST_ORIGIN_INDEX];

  /*

    ===== Create Bound Cubes =====

   */

  SceneEditorMesh *bound_cube = sem_list_new_entry(list);
  bound_cube->mesh = rem_new_mesh();
  bound_cube->target = grid;
  bound_cube->target_list_index = desc->target_list_index; // necessary ?
  bound_cube->scene = desc->scene;

  Primitive cube_primitive;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/cube.mbin",
      .primitive = &cube_primitive,
  });

  SEMCreateWireframeDescriptor wireframe_desc = {
      .color = &(color){1.0f, 0.0f, 0.0f, 1.0f},
      .index = &cube_primitive.index,
      .vertex = &cube_primitive.vertex,
      .name = "sem probe reflection bound",
      .thickness = SEM_WIREFRAME_LINE_THICKNESS,
  };

  vec3 padded_size;
  glm_vec3_scale(grid->scale, 2.2f, padded_size);
  sem_create_wireframe(bound_cube->mesh, &wireframe_desc);

  mesh_set_scale(bound_cube->mesh, padded_size);

  bound_cube->transform_callback[GizmoMode_Position] =
      sem_probe_reflection_grid_bound_set_position;
  bound_cube->transform_callback[GizmoMode_Rotation] =
      sem_probe_reflection_grid_set_rotation;
  bound_cube->transform_callback[GizmoMode_Scale] =
      sem_probe_reflection_grid_bound_set_scale;

  /*

    ===== Create Probes Cubes =====

   */
  for (size_t i = 0; i < grid->probes.length; i++) {

    SceneEditorMesh *probe = sem_list_new_entry(list);
    probe->mesh = rem_new_mesh();
    probe->target = &grid->probes.entries[i];
    probe->target_list_index = i;
    probe->scene = desc->scene;

    if (probe == NULL) {
      logger_add(LoggerFlag_Warning, "Couldn't create new mesh for probe SEM.");
      break;
    }

    sem_create_wireframe(probe->mesh, &wireframe_desc);

    mesh_set_scale(probe->mesh, (vec3){0.6f, 0.6f, 0.6f});
    mesh_set_position(probe->mesh, grid->probes.entries[i].position);

    probe->transform_callback[GizmoMode_Position] =
        sem_probe_reflection_grid_set_position;
    probe->transform_callback[GizmoMode_Rotation] =
        sem_probe_reflection_grid_set_rotation;
    probe->transform_callback[GizmoMode_Scale] =
        sem_probe_reflection_grid_set_scale;

    mesh_child_add(bound_cube->mesh, probe->mesh);
  }
}

/**
   Update the position list according to the origin on top the casual mesh
   translation.
 */
void sem_probe_reflection_grid_bound_set_position(SceneEditorMesh *sem,
                                                  vec3 value) {

  ProbeReflectionGrid *grid = (ProbeReflectionGrid *)sem->target;
  mesh_set_position(sem->mesh, value);
}

void sem_probe_reflection_grid_bound_set_scale(SceneEditorMesh *sem,
                                               vec3 value) {
  // mesh_set_scale(desc->mesh->mesh, desc->offset);
}

void sem_probe_reflection_grid_set_position(SceneEditorMesh *sem, vec3 value) {

  mesh_set_position(sem->mesh, value);

  ProbeReflection *probe = (ProbeReflection *)sem->target;
  glm_vec3_copy(sem->mesh->position, probe->position);

  // update uniform cpu side
  probe_reflection_update_uniform(probe);

  // add to upload queue
  ubo_update_queue_insert(&sem->scene->renderer.ubo, UBOType_ProbeList,
                          sem->scene->probes.ubo_slot.id);

  // update view cpu side
  probe_reflection_update_camera(probe);

  // add to upload queue
  for (uint8_t i = 0; i < PROBE_REFLECTION_VIEW_COUNT; i++)
    ubo_update_queue_insert(&sem->scene->renderer.ubo, UBOType_Camera,
                            probe->ubo_camera[i].id);
}

void sem_probe_reflection_grid_set_rotation(SceneEditorMesh *sem, vec3 value) {}

void sem_probe_reflection_grid_set_scale_(SceneEditorMesh *sem, vec3 value) {
  // print_vec3(desc->offset);
}
