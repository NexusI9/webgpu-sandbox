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

  // go through the mesh triangles and check if it's occluded
  for (size_t i = 0; i < mesh->topology.base.index.length; i += 3) {

    Triangle source_triangle = ao_bake_mesh_triangle(mesh, i);

    uint16_t sampling =
        glm_min(AO_GLOBAL_RAY_MAX_AMOUNT, desc->settings->sample_amount);

    vec3 rays[AO_GLOBAL_RAY_MAX_AMOUNT];
    vec3 ray_normal;

    triangle_normal(&source_triangle, ray_normal);
    glm_vec3_scale(ray_normal, desc->settings->max_distance, ray_normal);

    triangle_random_points(&source_triangle, sampling, rays);

    // create a ray on the triangle surface, projects it and check if it
    // collides with another mesh in the scene within a certain distance
    for (int ray = 0; ray < sampling; ray++) {

      vec3 ray_direction;
      glm_vec3_add(rays[ray], ray_normal, ray_direction);

      if (line && ray < desc->debug->max_ray) {
        line_add_point(rays[ray], ray_direction, (vec3){0.0f, 1.0f, 0.0f},
                       &line->topology.base.attribute,
                       &line->topology.base.index);
      }

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

  if (line && desc->debug->pipeline) {
    line_update_buffer(line);
    mesh_ref_list_insert(desc->debug->pipeline, line);
  }

#ifdef AO_BAKE_HIT_COUNT
  VERBOSE_DEBUG("%s hits: %d", mesh->name, g_debug_ao_bake_hit_count);
  g_debug_ao_bake_hit_count = 0;
#endif
}
