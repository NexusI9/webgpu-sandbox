#include "./global.h"

#include "../utils/system.h"
#include "./utils.h"
#include "core.h"
#include "texture_list.h"

/**
   Bake global ambient occlusion to texture, meaning occlusion based on
   meshes in a scene using a raycasting method.
 */
void ao_bake_global(SceneRendererTextureAO *ao,
                    const AOBakeGlobalDescriptor *desc) {

#ifdef AO_BAKE_DISPLAY_RAY
  mesh *line = scene_new_mesh_unlit(desc->scene, NULL);
  line_create(line, &(LineCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .name = "line mesh",
                    });
#endif

  Mesh *mesh = desc->mesh;
  VERBOSE_PROCESS("Baking Global AO for mesh: %s", mesh->name);

  // go through the mesh triangles and check if it's occluded
  for (size_t i = 0; i < mesh->topology.base.index.length; i += 3) {
    Triangle source_triangle = ao_bake_mesh_triangle(mesh, i);
    vec3 rays[AO_GLOBAL_RAY_AMOUNT];
    vec3 ray_normal;
    triangle_normal(&source_triangle, ray_normal);
    glm_vec3_scale(ray_normal, AO_GLOBAL_RAY_MAX_DISTANCE, ray_normal);

    triangle_random_points(&source_triangle, AO_GLOBAL_RAY_AMOUNT, rays);

    // create a ray on the triangle surface, projects it and check if it
    // collides with another mesh in the scene within a certain distance
    for (int ray = 0; ray < desc->settings->sample_amount; ray++) {

      vec3 ray_direction;
      glm_vec3_add(rays[ray], ray_normal, ray_direction);

#ifdef AO_BAKE_DISPLAY_RAY
      if (ray < AO_RAY_MAX_COUNT)
        line_add_point(rays[ray], ray_direction, (vec3){0.0f, 1.0f, 0.0f},
                       &line->vertex, &line->index);
#endif

      for (size_t c = 0; c < desc->mesh_list->length; c++) {
        Mesh *compare_mesh = desc->mesh_list->entries[c];

        if (mesh == compare_mesh)
          continue;

        Texture *compare_texture =
            ao_bake_texture_list_find(&ao->texture_list, compare_mesh, NULL);

        ao_bake_raycast(&(AOBakeRaycastDescriptor){
            .ray_origin = &rays[ray],
            .ray_direction = &ray_direction,
            .source_triangle = &source_triangle,
            .source_texture = desc->texture,
            .compare_texture = compare_texture,
            .compare_mesh = compare_mesh,
            .max_distance = desc->settings->max_distance,
        });
      }
    }
  }

#ifdef AO_BAKE_HIT_COUNT
  VERBOSE_DEBUG("%s hits: %d", mesh->name, g_debug_ao_bake_hit_count);
  g_debug_ao_bake_hit_count = 0;
#endif

#ifdef AO_BAKE_DISPLAY_RAY
  line_update_buffer(line);
#endif
}
