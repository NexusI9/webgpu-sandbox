#include "reflection_grid.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/scene/editor/object/object.h"
#include "../runtime/scene/scene.h"
#include <stdint.h>

void seo_probe_reflection_grid_create(SceneEditorObject *seo,
                                 ProbeReflectionGrid *grid,
                                 const SEOCreateDescriptor *desc) {

  seo->scene = desc->scene;

  // 1 bound cube + (x * y * z probes)
  const uint16_t seo_mesh_count =
      1 + grid->count[0] * grid->count[1] * grid->count[2];
  seo_mesh_list_create(&seo->meshes, seo_mesh_count);

  /*

    ===== Create Bound Cubes =====

   */

  SceneEditorObjectMesh *bound_cube = seo_mesh_list_new_entry(&seo->meshes);
  bound_cube->mesh = scene_new_mesh(desc->scene);
  bound_cube->target = grid;
  bound_cube->target_list_index = desc->target_list_index; // necessary ?
  Primitive cube_primitive;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/cube.mbin",
      .primitive = &cube_primitive,
  });

  SEOCreateWireframeDescriptor wireframe_desc = {
      .color = &(color){1.0f, 0.0f, 0.0f, 1.0f},
      .device = desc->device,
      .queue = desc->queue,
      .index = &cube_primitive.index,
      .vertex = &cube_primitive.vertex,
      .name = "seo probe reflection bound",
      .thickness = SEO_WIREFRAME_LINE_THICKNESS,
  };

  vec3 padded_size;
  glm_vec3_scale(grid->scale, 2.2f, padded_size);
  seo_create_wireframe(bound_cube->mesh, &wireframe_desc);

  mesh_set_scale(bound_cube->mesh, padded_size);

  bound_cube->transform_callback[GizmoMode_Position] =
      seo_probe_reflection_grid_bound_set_position;
  bound_cube->transform_callback[GizmoMode_Rotation] =
      seo_probe_reflection_grid_set_rotation;
  bound_cube->transform_callback[GizmoMode_Scale] =
      seo_probe_reflection_grid_bound_set_scale;

  /*

    ===== Create Probes Cubes =====

   */
  for (size_t i = 0; i < grid->probes.length; i++) {

    SceneEditorObjectMesh *probe = seo_mesh_list_new_entry(&seo->meshes);
    probe->mesh = scene_new_mesh(desc->scene);
    probe->target = &grid->probes.entries[i];
    probe->target_list_index = i;

    if (probe == NULL) {
      VERBOSE_WARNING("Couldn't create new mesh for probe SEO.");
      break;
    }

    seo_create_wireframe(probe->mesh, &wireframe_desc);

    mesh_set_scale(probe->mesh, (vec3){0.6f, 0.6f, 0.6f});
    mesh_set_position(probe->mesh, grid->probes.entries[i].position);

    probe->transform_callback[GizmoMode_Position] =
        seo_probe_reflection_grid_set_position;
    probe->transform_callback[GizmoMode_Rotation] =
        seo_probe_reflection_grid_set_rotation;
    probe->transform_callback[GizmoMode_Scale] =
        seo_probe_reflection_grid_set_scale;

    mesh_child_add(bound_cube->mesh, probe->mesh);
  }

  seo->origin = bound_cube->mesh;
}

/**
   Update the position list according to the origin on top the casual mesh
   translation.
 */
void seo_probe_reflection_grid_bound_set_position(SEOTransformCallback *desc) {

  ProbeReflectionGrid *grid = (ProbeReflectionGrid *)desc->mesh->target;
  mesh_set_position(desc->mesh->mesh, desc->offset);
}

void seo_probe_reflection_grid_bound_set_scale(SEOTransformCallback *desc) {
  // mesh_set_scale(desc->mesh->mesh, desc->offset);
}

void seo_probe_reflection_grid_set_position(SEOTransformCallback *desc) {

  mesh_set_position(desc->mesh->mesh, desc->offset);

  ProbeReflection *probe = (ProbeReflection *)desc->mesh->target;
  glm_vec3_copy(desc->mesh->mesh->position, probe->position);

  // update uniform cpu side
  probe_reflection_update_uniform(probe);

  // add to upload queue
  ssbo_update_queue_insert(&desc->seo->scene->renderer.ssbo,
                           SSBOType_ProbeGridReflection,
                           probe->ssbo_slot[ProbeReflectionSSBOField_List].id);

  // update view cpu side
  probe_reflection_update_view(probe);

  // add to upload queue
  ssbo_update_queue_insert(&desc->seo->scene->renderer.ssbo,
                           SSBOType_ViewProbeReflection,
                           probe->ssbo_slot[ProbeReflectionSSBOField_View].id);
}

void seo_probe_reflection_grid_set_rotation(SEOTransformCallback *desc) {}

void seo_probe_reflection_grid_set_scale(SEOTransformCallback *desc) {

  // print_vec3(desc->offset);
}
