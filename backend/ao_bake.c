#include "ao_bake.h"
#include "../resources/debug/line.h"
#include "../runtime/material.h"
#include "../utils/point.h"
#include "../utils/system.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <emscripten/emscripten.h>
#include <math.h>
#include <stdint.h>

static inline void ao_bake_global(const AOBakeDescriptor *desc);
static inline void ao_bake_local(const AOBakeDescriptor *desc);
static inline void ao_bake_raycast(const AOBakeRaycastDescriptor *);
static inline float ao_bake_vertex(vertex *, mesh *, mesh *);
static inline void ao_bake_bind(mesh *, texture *);
static triangle ao_bake_mesh_triangle(mesh *, size_t);

float ao_bake_vertex(vertex *vertex, mesh *source, mesh *line) {

  int vertex_hit = 0;
  vec3 rays[AO_LOCAL_RAY_AMOUNT];
  // Generate random ray in an hemisphere oriented on vertex normal
  hemisphere_random_points(vertex->normal, AO_LOCAL_RAY_AMOUNT, rays);

  vec3 world_position;
  glm_mat4_mulv3(source->model, vertex->position, 1.0f, world_position);

  for (int ray = 0; ray < AO_LOCAL_RAY_AMOUNT; ray++) {

    vec3 ray_direction;
    glm_vec3_scale(rays[ray], AO_LOCAL_RAY_MAX_DISTANCE, ray_direction);
    glm_vec3_add(world_position, ray_direction, ray_direction);

#ifdef AO_BAKE_DISPLAY_RAY
    if (line && ray < AO_RAY_MAX_COUNT)
      line_add_point(world_position, ray_direction, (vec3){0.0f, 1.0f, 0.0f},
                     &line->vertex, &line->index);
#endif

    // traverse mesh triangles
    for (size_t t = 0; t < source->vertex.base.index.data.length; t += 3) {
      triangle triangle = ao_bake_mesh_triangle(source, t);
      vec3 hit;
      triangle_raycast(&triangle, world_position, ray_direction,
                       AO_LOCAL_RAY_MAX_DISTANCE, hit);

      // is occluded
      if (hit[0] || hit[1] || hit[2])
        vertex_hit++;
    }
  }

  // accumulated AO
  return 1 - ((float)vertex_hit / AO_LOCAL_RAY_AMOUNT);
}

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
  for (size_t i = 0; i < desc->compare_mesh->vertex.base.index.data.length; i += 3) {
    triangle compare_triangle = ao_bake_mesh_triangle(desc->compare_mesh, i);
    vec3 hit;
    triangle_raycast(&compare_triangle, *desc->ray_origin, *desc->ray_direction,
                     AO_GLOBAL_RAY_MAX_DISTANCE, hit);

    // is occluded
    // transpose hit point to triangle UV space
    // 1. retrieve hit position and translate it to uv space
    // 2. scale to the texture coordinates
    // 3.write pixel to texture
    if (hit[0] || hit[1] || hit[2]) {
      vec2 compare_uv, source_uv;
      triangle_point_to_uv(desc->source_triangle, *desc->ray_origin, source_uv);
      glm_vec2_scale(source_uv, AO_TEXTURE_SIZE, source_uv);
      texture_write_pixel(desc->source_texture, 0, source_uv,
                          TextureWriteMethod_Replace);

      // do the same for compare mesh
      triangle_point_to_uv(&compare_triangle, hit, compare_uv);
      glm_vec2_scale(compare_uv, AO_TEXTURE_SIZE, compare_uv);
      texture_write_pixel(desc->compare_texture, 0, compare_uv,
                          TextureWriteMethod_Replace);
    }
  }
}

/**
   Return a triangle of a mesh starting at a certain index
 */
triangle ao_bake_mesh_triangle(mesh *mesh, size_t index) {

  float *base_attribute = mesh->vertex.base.attribute.data.entries;
  uint16_t *base_index = mesh->vertex.base.index.data.entries;

  vertex source_vertex_a =
      vertex_from_array(&base_attribute[base_index[index] * VERTEX_STRIDE]);

  vertex source_vertex_b =
      vertex_from_array(&base_attribute[base_index[index + 1] * VERTEX_STRIDE]);

  vertex source_vertex_c =
      vertex_from_array(&base_attribute[base_index[index + 2] * VERTEX_STRIDE]);

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

  // init textures
  texture ao_textures[desc->mesh_list->length];
  for (int t = 0; t < desc->mesh_list->length; t++) {
    texture_create(&ao_textures[t], &(TextureCreateDescriptor){
                                        .width = AO_TEXTURE_SIZE,
                                        .height = AO_TEXTURE_SIZE,
                                        .channels = TEXTURE_CHANNELS_R,
                                        .value = 255,
                                    });
  }

  // global ao generation
  ao_bake_global(&(AOBakeDescriptor){
      .device = desc->device,
      .queue = desc->queue,
      .mesh_list = desc->mesh_list,
      .scene = desc->scene,
      .texture = ao_textures,
  });

  // local ao generation
  ao_bake_local(&(AOBakeDescriptor){
      .device = desc->device,
      .queue = desc->queue,
      .mesh_list = desc->mesh_list,
      .scene = desc->scene,
      .texture = ao_textures,
  });

  // blur and bind textures once baking is done
  for (int s = 0; s < desc->mesh_list->length; s++) {
    mesh *source_mesh = desc->mesh_list->entries[s];
    texture_remap(&ao_textures[s], 0, 1, &ao_textures[s].data);

    // 1st pass blur
    texture_blur(&ao_textures[s], 3, 1.0f, &ao_textures[s].data);
    // 2nd pass blur
    texture_blur(&ao_textures[s], 3, 1.0f, &ao_textures[s].data);

    ao_bake_bind(source_mesh, &ao_textures[s]);
  }
}

/**
   Bake local ambient occlusion to texture, meaning occlusion based on mesh own
   vertices occlusion.
   Compared to the global baking, the local use a vertex
   based approach and act as a cavity map rather than a standard AO map.
 */
void ao_bake_local(const AOBakeDescriptor *desc) {

  VERBOSE_PRINT("===== BAKING LOCAL AO =====\n");

  mesh *line = NULL;
#ifdef AO_BAKE_DISPLAY_RAY
  line = scene_new_mesh_unlit(desc->scene);
  line_create(line, &(LineCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .name = "line mesh",
                    });
#endif

  for (size_t m = 0; m < desc->mesh_list->length; m++) {

    mesh *current_mesh = desc->mesh_list->entries[m];

    VERBOSE_PRINT("Baking mesh: %s\n", current_mesh->name);

    VertexAttribute *mesh_vertex = &current_mesh->vertex.base.attribute.data;
    VertexIndex *mesh_index = &current_mesh->vertex.base.index.data;
    texture *mesh_texture = &desc->texture[m];
    /*
      Go through each indexes
      since indexes are drawn sequentially we can compare 2 by 2 and interpolate
      the result between these two as to draw the cavity on the texture:

             AO(n) = 1.0       Interpolate
              x          <-------- 0.95
               '.         <------- 0.75
                 '.        <------ 0.50
                   '.       <----- 0.25
                     x
                      AO(n+1) = 0.0

       UPDATE: It kinda sucks as it clearly highlights the triangles too much
     */

    for (size_t i = 0; i < mesh_index->length; i += 3) {

      // calculate AO for vertex A
      size_t offset_a = mesh_index->entries[i] * VERTEX_STRIDE;
      vertex vertex_a = vertex_from_array(&mesh_vertex->entries[offset_a]);
      float ao_a = ao_bake_vertex(&vertex_a, current_mesh, line);
      vec2 uv_a;
      glm_vec2_scale(vertex_a.uv, AO_TEXTURE_SIZE, uv_a);

      // calculate AO for vertex B
      size_t offset_b = mesh_index->entries[i + 1] * VERTEX_STRIDE;
      vertex vertex_b = vertex_from_array(&mesh_vertex->entries[offset_b]);
      float ao_b = ao_bake_vertex(&vertex_b, current_mesh, line);
      vec2 uv_b;
      glm_vec2_scale(vertex_b.uv, AO_TEXTURE_SIZE, uv_b);

      // calculate AO for vertex C
      size_t offset_c = mesh_index->entries[i + 2] * VERTEX_STRIDE;
      vertex vertex_c = vertex_from_array(&mesh_vertex->entries[offset_c]);
      float ao_c = ao_bake_vertex(&vertex_c, current_mesh, line);
      vec2 uv_c;
      glm_vec2_scale(vertex_c.uv, AO_TEXTURE_SIZE, uv_c);

      // if at least on vertex is occluded
      if (ao_a + ao_b + ao_c > 0) {

        /* Draw line on each vertex and bridges due to UV seams

           3D View:
                     '-.  A,C  .-'
                     |  '-.o.-'  |
                     |     x     |
                     |	   x <--------- UV seam of AB/CD
                     ',    x    ,'
                       '-._o_.-'
                          B,D
           UV View:

          0                                     +1
           +------------------------------------>
           |    A                          C
           |     o._                    _.o
           |	 x  '-.______________.-'  x
           |     x    |              |    x
           |     x    |              |    x
           |     x    |              |    x
           |     x    |              |    x
           |     x    |              |    x
           |     x    |              |    x
           |     x _.-+--------------+-._ x
           |     o'                      'o
           |    B                           D
           v
         +1

             Need to draw line on both AB and CD
        */

        // create a temporary texture to add it to the already baked texture
        // or create an WriteMethod enum like:
        // replace | add | multiply
        // and ajdjust the pixel[c] = ... accordignly
        texture_write_triangle_gradient(
            &(TextureWriteTriangleGradientDescriptor){
                .source = mesh_texture,
                .destination = &mesh_texture->data,
                .length = 1,
                .write_method = TextureWriteMethod_Mul,
                .points =
                    (TextureTriangleGradientDescriptor[]){
                        {
                            .a =
                                {
                                    .position = {uv_a[0], uv_a[1]},
                                    .value = &(float){ao_a},
                                },
                            .b =
                                {
                                    .position = {uv_b[0], uv_b[1]},
                                    .value = &(float){ao_b},
                                },
                            .c =
                                {
                                    .position = {uv_c[0], uv_c[1]},
                                    .value = &(float){ao_c},
                                },
                        },
                    },
            });
      }
    }
  }

#ifdef AO_BAKE_DISPLAY_RAY
  line_update_buffer(line);
  material_texture_bind_views(line, &desc->scene->camera,
                              &desc->scene->viewport, 0);
#endif
}

/**
   Bake global ambient occlusion to texture, meaning occlusion based on
   meshes in a scene using a raycasting method.
 */
void ao_bake_global(const AOBakeDescriptor *desc) {

  VERBOSE_PRINT("===== BAKING GLOBAL AO =====\n");

#ifdef AO_BAKE_DISPLAY_RAY
  mesh *line = scene_new_mesh_unlit(desc->scene);
  line_create(line, &(LineCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .name = "line mesh",
                    });
#endif

  // traverse list
  for (size_t s = 0; s < desc->mesh_list->length; s++) {

    mesh *source_mesh = desc->mesh_list->entries[s];
    VERBOSE_PRINT("Baking mesh: %s\n", source_mesh->name);

    // go through the mesh triangles and check if it's occluded
    for (size_t i = 0; i < source_mesh->vertex.base.index.data.length; i += 3) {
      triangle source_triangle = ao_bake_mesh_triangle(source_mesh, i);
      vec3 rays[AO_GLOBAL_RAY_AMOUNT];
      vec3 ray_normal;
      triangle_normal(&source_triangle, ray_normal);
      glm_vec3_scale(ray_normal, AO_GLOBAL_RAY_MAX_DISTANCE, ray_normal);

      triangle_random_points(&source_triangle, AO_GLOBAL_RAY_AMOUNT, rays);

      // create a ray on the triangle surface, projects it and check if it
      // collides with another mesh in the scene within a certain distance
      for (int ray = 0; ray < AO_GLOBAL_RAY_AMOUNT; ray++) {

        vec3 ray_direction;
        glm_vec3_add(rays[ray], ray_normal, ray_direction);

#ifdef AO_BAKE_DISPLAY_RAY
        if (ray < AO_RAY_MAX_COUNT)
          line_add_point(rays[ray], ray_direction, (vec3){0.0f, 1.0f, 0.0f},
                         &line->vertex, &line->index);
#endif

        for (size_t c = 0; c < desc->mesh_list->length; c++) {

          mesh *compare_mesh = desc->mesh_list->entries[c];

#ifndef AO_GLOBAL_SELF
          // TODO: once the index system is properly setup, replace m == s
          // src id == compare id
          if (strcmp(source_mesh->name, compare_mesh->name) == 0)
            continue;
#endif

          ao_bake_raycast(&(AOBakeRaycastDescriptor){
              .ray_origin = &rays[ray],
              .ray_direction = &ray_direction,
              .source_triangle = &source_triangle,
              .source_texture = &desc->texture[s],
              .compare_texture = &desc->texture[c],
              .compare_mesh = compare_mesh,
          });
        }
      }
    }
  }

#ifdef AO_BAKE_DISPLAY_RAY
  line_update_buffer(line);
  material_texture_bind_views(line, &desc->scene->camera,
                              &desc->scene->viewport, 0);
#endif
}
