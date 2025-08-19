#include "reflection.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"
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
  mesh_ref_list_create(&seo->meshes, seo_mesh_count);

  /*

    ===== Create Bound Cubes =====

   */

  Mesh *bound_cube = scene_new_mesh(desc->scene);
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
  seo_create_wireframe(bound_cube, &wireframe_desc);

  mesh_scale(bound_cube, padded_size);

  mesh_ref_list_insert(&seo->meshes, bound_cube);

  /*

    ===== Create Probes Cubes =====

   */
  for (size_t i = 0; i < grid->position.length; i++) {

    Mesh *probe = scene_new_mesh(desc->scene);
    if (probe == NULL) {
      VERBOSE_WARNING("Couldn't create new mesh for probe SEO.");
      break;
    }

    seo_create_wireframe(probe, &wireframe_desc);

    mesh_scale(probe, (vec3){0.3f, 0.3f, 0.3f});
    mesh_translate(probe, grid->position.entries[i]);
    mesh_ref_list_insert(&seo->meshes, probe);
  }

  seo->transform_callback[GizmoTransformMode_Translate] =
      seo_probe_reflection_translate;
  seo->transform_callback[GizmoTransformMode_Rotate] =
      seo_probe_reflection_rotate;
  seo->transform_callback[GizmoTransformMode_Scale] =
      seo_probe_reflection_scale;
}

void seo_probe_reflection_translate(SceneEditorObject *seo, vec3 value) {}
void seo_probe_reflection_rotate(SceneEditorObject *seo, vec3 value) {}
void seo_probe_reflection_scale(SceneEditorObject *seo, vec3 value) {}
