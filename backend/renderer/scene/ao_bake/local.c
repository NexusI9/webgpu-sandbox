#include "./local.h"
#include "../utils/system.h"
#include "./utils.h"

/**
   Bake local ambient occlusion to texture, meaning occlusion based on mesh own
   vertices occlusion.
   Compared to the global baking, the local use a vertex
   based approach and act as a cavity map rather than a standard AO map.
 */
void ao_bake_local(SceneRendererTextureAO *ao,
                   const AOBakeLocalDescriptor *desc) {

  Mesh *line = NULL;
#ifdef AO_BAKE_DISPLAY_RAY
  line = scene_new_mesh(desc->scene, NULL);
  line_create(line, &(LineCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .name = "line mesh",
                    });
  scene_add_mesh(scene, line, ScenePipeline_Dynamic_Unlit, NULL);
#endif

  Mesh *mesh = desc->mesh;

  VERBOSE_PROCESS("Baking Local AO for mesh: %s", mesh->name);

  VertexAttribute *mesh_vertex = &mesh->topology.base.attribute;
  VertexIndex *mesh_index = &mesh->topology.base.index;

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
    Vertex vertex_a = vertex_from_array(&mesh_vertex->entries[offset_a]);
    float ao_a = ao_bake_vertex(&vertex_a, mesh, line);
    vec2 uv_a;
    glm_vec2_scale(vertex_a.uv, AO_TEXTURE_SIZE, uv_a);

    // calculate AO for vertex B
    size_t offset_b = mesh_index->entries[i + 1] * VERTEX_STRIDE;
    Vertex vertex_b = vertex_from_array(&mesh_vertex->entries[offset_b]);
    float ao_b = ao_bake_vertex(&vertex_b, mesh, line);
    vec2 uv_b;
    glm_vec2_scale(vertex_b.uv, AO_TEXTURE_SIZE, uv_b);

    // calculate AO for vertex C
    size_t offset_c = mesh_index->entries[i + 2] * VERTEX_STRIDE;
    Vertex vertex_c = vertex_from_array(&mesh_vertex->entries[offset_c]);
    float ao_c = ao_bake_vertex(&vertex_c, mesh, line);
    vec2 uv_c;
    glm_vec2_scale(vertex_c.uv, AO_TEXTURE_SIZE, uv_c);

    // if at least on vertex is occluded
    if (ao_a + ao_b + ao_c > 0) {

#ifdef AO_BAKE_HIT_COUNT
      g_debug_ao_bake_hit_count++;
#endif

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
         |     x  '-.______________.-'  x
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

      texture_write_triangle_gradient(&(TextureWriteTriangleGradientDescriptor){
          .source = desc->texture,
          .destination = &desc->texture->data,
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

#ifdef AO_BAKE_HIT_COUNT
  VERBOSE_DEBUG("%s hits: %d", mesh->name, g_debug_ao_bake_hit_count);
  g_debug_ao_bake_hit_count = 0;
#endif

#ifdef AO_BAKE_DISPLAY_RAY
  line_update_buffer(line);
#endif
}
