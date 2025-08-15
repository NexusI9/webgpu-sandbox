#include "./global.h"

#include "../runtime/geometry/line/line.h"
#include "../runtime/mesh/shader/shader.h"
#include "../utils/system.h"
#include "./utils.h"
#include "core.h"
#include "texture_list.h"
#include <stdint.h>

/**
   Bake global ambient occlusion to texture, meaning occlusion based on
   meshes in a scene using a raycasting method.
 */
void ao_bake_global(SceneRendererTextureAO *ao,
                    const AOBakeGlobalDescriptor *desc) {

  Mesh *line = NULL;
  if (desc->debug->meshes) {
    line = mesh_list_new_mesh(desc->debug->meshes);
    line_create(line, &(LineCreateDescriptor){
                          .device = desc->device,
                          .queue = desc->queue,
                          .name = "line mesh",
                      });

    mesh_shader_build_mvp(line, mesh_shader_fixed, desc->debug->camera,
                          desc->debug->viewport);
  }

  Mesh *mesh = desc->mesh;
  VERBOSE_PROCESS("Baking Global AO for mesh: %s", mesh->name);

  // COMPARE MESH
  for (size_t c = 0; c < desc->mesh_list->length; c++) {

    Mesh *compare_mesh = desc->mesh_list->entries[c];

    if (mesh == compare_mesh ||
        !aabb_within_distance(&mesh->topology.boundbox.bound,
                              &compare_mesh->topology.boundbox.bound,
                              desc->settings->max_distance, NULL))

      continue;

    Texture *compare_texture =
        ao_bake_texture_list_find(&ao->texture_list, compare_mesh, NULL);

    // go through the mesh triangles and check if it's occluded
    for (size_t i = 0; i < mesh->topology.base.index.length; i += 3) {

      Triangle source_triangle;
      ao_bake_mesh_triangle(&source_triangle, mesh, i);

      AABB tri_aabb;
      vec3 tri_points[3];
      glm_vec3_copy(source_triangle.a.position, tri_points[0]);
      glm_vec3_copy(source_triangle.b.position, tri_points[1]);
      glm_vec3_copy(source_triangle.c.position, tri_points[2]);

      aabb_from_vec3(&tri_aabb, tri_points, 3);

      if (!aabb_within_distance(&tri_aabb,
                                &compare_mesh->topology.boundbox.bound,
                                desc->settings->max_distance, NULL))
        continue;

      uint16_t sampling =
          glm_min(AO_GLOBAL_RAY_MAX_AMOUNT, desc->settings->sample_amount);

      vec3 rays[AO_GLOBAL_RAY_MAX_AMOUNT];
      vec3 ray_normal, ray_scaled_normal;

      triangle_normal(&source_triangle, ray_normal);
      glm_vec3_scale(ray_normal, desc->settings->max_distance,
                     ray_scaled_normal);

      triangle_random_points(&source_triangle, sampling, rays);

      // RAY SAMPLES
      // create a ray on the triangle surface, projects it and check if it
      // collides with another mesh in the scene within a certain distance
      for (int ray = 0; ray < sampling; ray++) {
        vec3 ray_direction;
        glm_vec3_sub(ray_normal, rays[ray], ray_direction);
        glm_normalize(ray_direction);

        vec3 color = {0.0f, 1.0f, 0.0f};

        if (ao_bake_raycast(&(AOBakeRaycastDescriptor){
                .ray_origin = &rays[ray],
                .ray_direction = &ray_direction,
                .source_triangle = &source_triangle,
                .source_texture = desc->texture,
                .compare_texture = compare_texture,
                .compare_mesh = compare_mesh,
                .max_distance = desc->settings->max_distance,
                .texture_size = ao->size,
            }))
          glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, color); // red

        if (line && ray < desc->debug->max_ray) {
          vec3 ray_target;
          glm_vec3_add(ray_scaled_normal, rays[ray], ray_target);
          line_add_point(rays[ray], ray_target, color,
                         &line->topology.base.attribute,
                         &line->topology.base.index);
        }
      }
    }
  }

  if (line && desc->debug->pipeline) {
    line_update_buffer(line);
    mesh_ref_list_insert(desc->debug->pipeline, line);
  }

#ifdef AO_BAKE_HIT_COUNT
  VERBOSE_DEBUG("%s hits: %d", mesh->name, g_debug_ao_bake_hit_count);
  g_debug_ao_bake_hit_count = 0;
#endif
}
