#include "ao_bake.h"
#include "../geometry/triangle.h"
#include "../utils/system.h"
#include "string.h"
#include <cglm/cglm.h>
#include <emscripten/emscripten.h>
#include <stdint.h>

static void ao_bake_global(mesh *, MeshList *);
static void ao_bake_local(mesh *, MeshList *);
static void ao_bake_compare_triangle(triangle *, mesh *);
static triangle ao_bake_mesh_triangle(mesh *, size_t);

/**
   Take in the source mesh current triangle as well as a mesh
   The function then compare the position and normal of each triangles
   to check if the face of the source triangle or the compare mesh is occluded
 */
void ao_bake_compare_triangle(triangle *source, mesh *compare) {

  for (size_t i = 0; i < compare->index.length; i += 3) {

    triangle compare_triangle = ao_bake_mesh_triangle(compare, i);
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
  glm_mat4_mulv3(mesh->model, source_vertex_b.position, 1.0f,
                 source_vertex_b.position);

  return (triangle){
      .a = source_vertex_a,
      .b = source_vertex_b,
      .c = source_vertex_c,
  };
}

void ao_bake_init(const AOBakeInitDescriptor *desc) {

  printf("===== BAKING AO =====\n");

  for (int s = 0; s < desc->mesh_list->length; s++) {
    mesh *source_mesh = &desc->mesh_list->items[s];
    printf("Baking mesh: %s\n", source_mesh->name);

    // go through the mesh triangles and check if it's occluded

    for (size_t i = 0; i < source_mesh->index.length; i += 3) {

      triangle source_triangle = ao_bake_mesh_triangle(source_mesh, i);
      vec3 rays[AO_RAY_AMOUNT];
      triangle_random_points(&source_triangle, AO_RAY_AMOUNT, rays);

      // create a ray on the triangle surface, projects it and check if it
      // collides with another mesh in the scene within a certain distance
      for (uint16_t ray = 0; ray < AO_RAY_AMOUNT; ray++) {

        if (ray == 0)
          print_vec3(rays[ray]);
       

        for (int c = 0; c < desc->mesh_list->length; c++) {

          // TODO: once the index system is properly setup, replace m == s by
          // src id
          // == compare id
          if (c == s)
            continue;

          mesh *compare_mesh = &desc->mesh_list->items[c];
          ao_bake_compare_triangle(&source_triangle, compare_mesh);

          // check children
          for (size_t m = 0; m < compare_mesh->children.length; m++)
            ao_bake_compare_triangle(&source_triangle,
                                     &compare_mesh->children.items[m]);
        }
      }
    }

    if (source_mesh->children.length > 0) {
      ao_bake_init(&(AOBakeInitDescriptor){
          .mesh_list = &source_mesh->children,
      });
    }
  }
}
