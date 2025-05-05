#include "ao_bake.h"
#include "../resources/debug/line.h"
#include "../runtime/material.h"
#include "../utils/system.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <emscripten/emscripten.h>
#include <math.h>
#include <stdint.h>

static inline void ao_bake_global(mesh *, MeshList *);
static inline void ao_bake_local(mesh *, MeshList *);
static inline void ao_bake_raycast(const AOBakeRaycastDescriptor *);
static inline void ao_bake_bind(mesh *, texture *);
static triangle ao_bake_mesh_triangle(mesh *, size_t);

/**
   Bind the texture to the shader
 */
void ao_bake_bind(mesh *mesh, texture *texture) {

  shader_add_texture(mesh_shader_texture(mesh),
                     &(ShaderCreateTextureDescriptor){
                         .group_index = 0,
                         .entry_count = 1,
                         .entries = (ShaderBindGroupTextureEntry[]){
                             {
                                 .binding = 8,
                                 .data = texture->data,
                                 .size = texture->size,
                                 .width = texture->width,
                                 .height = texture->height,
                                 .dimension = WGPUTextureViewDimension_2D,
                                 .format = AO_TEXTURE_FORMAT,
                                 .sample_type = WGPUTextureSampleType_Float,
                                 .channels = AO_TEXTURE_CHANNELS,
                             },
                         }});

  shader_add_sampler(mesh_shader_texture(mesh),
                     &(ShaderCreateSamplerDescriptor){
                         .group_index = 0,
                         .entry_count = 1,
                         .entries = (ShaderBindGroupSamplerEntry[]){
                             {
                                 .binding = 9,
                                 .type = WGPUSamplerBindingType_Filtering,
                                 .addressModeU = WGPUAddressMode_ClampToEdge,
                                 .addressModeV = WGPUAddressMode_ClampToEdge,
                                 .addressModeW = WGPUAddressMode_ClampToEdge,
                                 .minFilter = WGPUFilterMode_Linear,
                                 .magFilter = WGPUFilterMode_Linear,
                                 .compare = WGPUCompareFunction_Undefined,
                             },
                         }});
}

/**
   Raycast from the source surage towards a certain direction an check if the
   ray traverse a triangle of the compared mesh
 */
void ao_bake_raycast(const AOBakeRaycastDescriptor *desc) {

  // Raycast from ray origin (source surface) towards each compare mesh
  // triangles
  for (size_t i = 0; i < desc->compare_mesh->index.length; i += 3) {
    triangle compare_triangle = ao_bake_mesh_triangle(desc->compare_mesh, i);
    vec3 hit;

    triangle_raycast(&compare_triangle, *desc->ray_origin, *desc->ray_direction,
                     AO_RAY_MAX_DISTANCE, hit);

    // is occluded
    if (hit[0] || hit[1] || hit[2]) {
      // transpose hit point to triangle UV space
      vec2 compare_uv, source_uv;

      triangle_point_to_uv(desc->source_triangle, *desc->ray_origin, source_uv);
      // scale to the texture coordinates
      glm_vec2_scale(source_uv, AO_TEXTURE_SIZE, source_uv);
      // write pixel to texture
      texture_write_pixel(desc->source_texture, 0, source_uv);

      // do the same for compare mesh
      triangle_point_to_uv(&compare_triangle, hit, compare_uv);
      glm_vec2_scale(compare_uv, AO_TEXTURE_SIZE, compare_uv);
      texture_write_pixel(desc->compare_texture, 0, compare_uv);
    }
  }
}

/**
   Return a triangle of a mesh starting at a certain index
 */
triangle ao_bake_mesh_triangle(mesh *mesh, size_t index) {

  vertex source_vertex_a = vertex_from_array(
      &mesh->vertex.data[mesh->index.data[index] * VERTEX_STRIDE]);

  vertex source_vertex_b = vertex_from_array(
      &mesh->vertex.data[mesh->index.data[index + 1] * VERTEX_STRIDE]);

  vertex source_vertex_c = vertex_from_array(
      &mesh->vertex.data[mesh->index.data[index + 2] * VERTEX_STRIDE]);

  // put vertex to worldspace
  glm_mat4_mulv3(mesh->model, source_vertex_a.position, 1.0f,
                 source_vertex_a.position);
  glm_mat4_mulv3(mesh->model, source_vertex_b.position, 1.0f,
                 source_vertex_b.position);
  glm_mat4_mulv3(mesh->model, source_vertex_c.position, 1.0f,
                 source_vertex_c.position);

  return (triangle){
      .a = source_vertex_a,
      .b = source_vertex_b,
      .c = source_vertex_c,
  };
}

void ao_bake_init(const AOBakeInitDescriptor *desc) {

  printf("===== BAKING AO =====\n");

#ifdef AO_BAKE_DISPLAY_RAY
  mesh *line = scene_new_mesh_unlit(desc->scene);
  line_create(line, &(LineCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .name = "line mesh",
                    });
#endif

  texture ao_texture[desc->mesh_list->length];
  // init textures
  for (int t = 0; t < desc->mesh_list->length; t++) {
    texture_create(&ao_texture[t], &(TextureCreateDescriptor){
                                       .width = AO_TEXTURE_SIZE,
                                       .height = AO_TEXTURE_SIZE,
                                       .channels = TEXTURE_CHANNELS_R,
                                       .value = 255,
                                   });
  }

  // traverse list
  for (int s = 0; s < desc->mesh_list->length; s++) {

    mesh *source_mesh = desc->mesh_list->entries[s];
    printf("Baking mesh: %s\n", source_mesh->name);

    // go through the mesh triangles and check if it's occluded
    for (size_t i = 0; i < source_mesh->index.length; i += 3) {
      triangle source_triangle = ao_bake_mesh_triangle(source_mesh, i);
      vec3 rays[AO_RAY_AMOUNT];
      vec3 ray_normal;
      triangle_normal(&source_triangle, ray_normal);
      glm_vec3_scale(ray_normal, AO_RAY_MAX_DISTANCE, ray_normal);

      triangle_random_points(&source_triangle, AO_RAY_AMOUNT, rays);

      // create a ray on the triangle surface, projects it and check if it
      // collides with another mesh in the scene within a certain distance
      for (uint16_t ray = 0; ray < AO_RAY_AMOUNT; ray++) {

        // vec2 ray_uv;
        // triangle_point_to_uv(&source_triangle, rays[ray], ray_uv);
        // glm_vec2_scale(ray_uv, AO_TEXTURE_SIZE, ray_uv);
        // texture_write_pixel(&ao_texture[s], 0, ray_uv);

        vec3 ray_direction;
        glm_vec3_add(rays[ray], ray_normal, ray_direction);

#ifdef AO_BAKE_DISPLAY_RAY
        if (ray < AO_RAY_MAX_COUNT)
          line_add_point(line, rays[ray], ray_direction, ray_normal);
#endif

        for (size_t c = 0; c < desc->mesh_list->length; c++) {

          mesh *compare_mesh = desc->mesh_list->entries[c];
          // TODO: once the index system is properly setup, replace m == s

          // src id == compare id
          if (strcmp(source_mesh->name, compare_mesh->name) == 0)
            continue;

          ao_bake_raycast(&(AOBakeRaycastDescriptor){
              .ray_origin = &rays[ray],
              .ray_direction = &ray_direction,
              .source_triangle = &source_triangle,
              .source_texture = &ao_texture[s],
              .compare_texture = &ao_texture[c],
              .compare_mesh = compare_mesh,
          });
        }
      }
    }

    texture_blur(&ao_texture[s], 3, 2.0f, &ao_texture[s].data);
    ao_bake_bind(source_mesh, &ao_texture[s]);
  }

#ifdef AO_BAKE_DISPLAY_RAY
  material_texture_bind_views(line, &desc->scene->camera,
                              &desc->scene->viewport, 0);
#endif
}
