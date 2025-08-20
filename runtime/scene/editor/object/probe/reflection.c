#include "reflection.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/scene/editor/object/object.h"
#include "../runtime/scene/scene.h"
#include <stdint.h>

void seo_probe_reflection_create(SceneEditorObject *seo,
                                 ProbeReflectionGrid *grid,
                                 const SEOCreateDescriptor *desc) {

  seo->target = grid;
  seo->scene = desc->scene;
  seo->target_list_index = desc->target_list_index; // necessary ?

  // 1 bound cube + (x * y * z probes)
  const uint16_t seo_mesh_count =
      1 + grid->count[0] * grid->count[1] * grid->count[2];
  seo_mesh_list_create(&seo->meshes, seo_mesh_count);

  /*

    ===== Create Bound Cubes =====

   */

  SceneEditorObjectMesh *bound_cube = seo_mesh_list_new_entry(&seo->meshes);
  bound_cube->mesh = scene_new_mesh(desc->scene);
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
  glm_vec3_scale(grid->size, 1.2f, padded_size);
  seo_create_wireframe(bound_cube->mesh, &wireframe_desc);

  mesh_set_scale(bound_cube->mesh, padded_size);

  bound_cube->transform_callback[GizmoTransformMode_Position] =
      seo_probe_reflection_bound_set_position;
  bound_cube->transform_callback[GizmoTransformMode_Rotation] =
      seo_probe_reflection_set_rotation;
  bound_cube->transform_callback[GizmoTransformMode_Scale] =
      seo_probe_reflection_bound_set_scale;

  /*

    ===== Create Probes Cubes =====

   */
  for (size_t i = 0; i < grid->position.length; i++) {

    SceneEditorObjectMesh *probe = seo_mesh_list_new_entry(&seo->meshes);
    probe->mesh = scene_new_mesh(desc->scene);

    if (probe == NULL) {
      VERBOSE_WARNING("Couldn't create new mesh for probe SEO.");
      break;
    }

    seo_create_wireframe(probe->mesh, &wireframe_desc);

    mesh_set_scale(probe->mesh, (vec3){0.3f, 0.3f, 0.3f});
    mesh_set_position(probe->mesh, grid->position.entries[i]);

    probe->transform_callback[GizmoTransformMode_Position] =
        seo_probe_reflection_set_position;
    probe->transform_callback[GizmoTransformMode_Rotation] =
        seo_probe_reflection_set_rotation;
    probe->transform_callback[GizmoTransformMode_Scale] =
        seo_probe_reflection_set_scale;

    mesh_child_add(bound_cube->mesh, probe->mesh);
  }

  seo->origin = bound_cube->mesh;
}

/**
   Update the position list according to the origin on top the casual mesh
   translation.
 */
void seo_probe_reflection_bound_set_position(Mesh *mesh, SceneEditorObject *seo,
                                             vec3 offset) {

  ProbeReflectionGrid *grid = (ProbeReflectionGrid *)seo->target;
  mesh_set_position(mesh, offset);

  for(size_t i = 0; i < grid->position.length; i++){
    
  }
}

void seo_probe_reflection_bound_set_scale(Mesh *mesh, SceneEditorObject *seo,
                                          vec3 offset) {
    mesh_set_scale(mesh, offset);
}

void seo_probe_reflection_set_position(Mesh *mesh, SceneEditorObject *seo,
                                       vec3 offset) {
  mesh_set_position(mesh, offset);
}

void seo_probe_reflection_set_rotation(Mesh *mesh, SceneEditorObject *seo,
                                       vec3 value) {}

void seo_probe_reflection_set_scale(Mesh *mesh, SceneEditorObject *seo,
                                    vec3 value) {
  
}
