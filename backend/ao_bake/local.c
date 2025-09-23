#include "./local.h"

#include <cglm/types.h>
#include <cglm/vec2.h>
#include <stddef.h>

#include "backend/logger.h"
#include "./utils.h"
#include "core.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/core.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/debug/ray.h"
#include "runtime/texture/write.h"

/**
   Bake local ambient occlusion to texture, meaning occlusion based on mesh own
   vertices occlusion.
   Compared to the global baking, the local use a vertex
   based approach and act as a cavity map rather than a standard AO map.
 */
void ao_bake_local(SceneRendererTextureAO *ao,
                   const AOBakeLocalDescriptor *desc) {

  Mesh *line = NULL;
  if (desc->debug->debug_scene)
    scene_debug_ray_create(desc->debug->debug_scene, &line);

  Mesh *mesh = desc->mesh;

  logger_add(LoggerFlag_Process, "Baking Local AO for mesh: %s", mesh->name);

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

    float ao_factor[3]; // point a,b,c ao factor
    vec3 uv[3];
    float sum = 0;

    for (size_t j = 0; j < 3; j++) {
      size_t offset = mesh_index->entries[i + j] * VERTEX_STRIDE;
      Vertex vertex = vertex_from_array(&mesh_vertex->entries[offset]);
      ao_factor[j] = ao_bake_vertex(&(AOBakeVertexDescriptor){
          .vertex = &vertex,
          .mesh = mesh,
          .debug =
              {
                  .line = line,
                  .max_ray = desc->debug->max_ray,
              },
          .settings = desc->settings,
      });
      glm_vec2_scale(vertex.uv, ao->size, uv[j]);
      sum += ao_factor[j];
    }

    // if at least on vertex is occluded
    if (sum > 0) {

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
                              .position = {uv[0][0], uv[0][1]},
                              .value = &(float){ao_factor[0]},
                          },
                      .b =
                          {
                              .position = {uv[1][0], uv[1][1]},
                              .value = &(float){ao_factor[1]},
                          },
                      .c =
                          {
                              .position = {uv[2][0], uv[2][1]},
                              .value = &(float){ao_factor[2]},
                          },
                  },
              },
      });
    }
  }

  if (line && desc->debug->debug_scene)
    scene_debug_ray_build(desc->debug->debug_scene, line);

#ifdef AO_BAKE_HIT_COUNT
  logger_add(LoggerFlag_Debug, "%s hits: %d", mesh->name, g_debug_ao_bake_hit_count);
  g_debug_ao_bake_hit_count = 0;
#endif
}
