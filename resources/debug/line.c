#include "line.h"
#include "../utils/system.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include <stdio.h>

static void line_push_vertex(const vec3, const vec3, const size_t, float *);
static void line_create_plane(const LineCreatePlaneDescriptor *);

void line_create(mesh *mesh, const LineCreateDescriptor *desc) {

  mesh_create(mesh, &(MeshCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .vertex = (VertexAttribute){0},
                        .index = (VertexIndex){0},
                        .name = desc->name,
                    });

  mesh_set_vertex_attribute(
      mesh, &(VertexAttribute){
                .data = calloc(LINE_MAX_POINTS * VERTEX_STRIDE, sizeof(float)),
                .length = 0,
            });

  mesh_set_vertex_index(mesh,
                        &(VertexIndex){
                            .data = calloc(LINE_MAX_POINTS, sizeof(uint16_t)),
                            .length = 0,
                        });

  mesh_set_shader(mesh,
                  &(ShaderCreateDescriptor){
                      .path = "./runtime/assets/shader/shader.default.wgsl",
                      .label = "line",
                      .name = "line",
                      .device = desc->device,
                      .queue = desc->queue,
                  });

  pipeline_set_primitive(shader_pipeline(mesh_shader_texture(mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });
}

void line_push_vertex(const vec3 position, const vec4 color,
                      const size_t offset, float *data) {

  // build up a new vertex with null normals and uv, but valid colors and
  // position

  // set position
  data[offset + 0] = position[0];
  data[offset + 1] = position[1];
  data[offset + 2] = position[2];

  // set normal
  data[offset + 3] = 0.0f;
  data[offset + 4] = 1.0f;
  data[offset + 5] = 0.0f;

  // set color
  data[offset + 6] = color[0];
  data[offset + 7] = color[1];
  data[offset + 8] = color[2];

  // set UV
  data[offset + 9] = 0.0f;
  data[offset + 10] = 0.0f;
}

void line_create_plane(const LineCreatePlaneDescriptor *desc) {

  /*

          Extrude along cross product w/ up vector
         ------------->

       A (p1)           B (p1-thickness)
        +--------------+
        ||'-           |
        ||  '_         |
        ||    '-       |
        ||      '-     |
        ||        '-   |
        ||          '- |
        +--------------+
       D (p2)	        C (p2-thickness)


   */

  size_t vertex_offset = desc->vertex->length;
  size_t vertex_count = 4;

  // add points to the array contiguously

  // A (p1)
  line_push_vertex(desc->points[0], desc->color, vertex_offset,
                   desc->vertex->data);

  // B (p1 thickness)
  line_push_vertex(desc->points[1], desc->color, vertex_offset + VERTEX_STRIDE,
                   desc->vertex->data);

  // C (p2 thickness)
  line_push_vertex(desc->points[2], desc->color,
                   vertex_offset + 2 * VERTEX_STRIDE, desc->vertex->data);

  // D (p2)
  line_push_vertex(desc->points[3], desc->color,
                   vertex_offset + 3 * VERTEX_STRIDE, desc->vertex->data);

  // add indices (A-B-C & A-C-D)
  size_t vertex_length = desc->vertex->length / VERTEX_STRIDE;

  desc->index->data[desc->index->length] = (uint16_t)vertex_length;
  desc->index->data[desc->index->length + 1] = (uint16_t)vertex_length + 1;
  desc->index->data[desc->index->length + 2] = (uint16_t)vertex_length + 2;
  desc->index->data[desc->index->length + 3] = (uint16_t)vertex_length + 2;
  desc->index->data[desc->index->length + 4] = (uint16_t)vertex_length + 3;
  desc->index->data[desc->index->length + 5] = (uint16_t)vertex_length;

  // update index length
  desc->index->length += 6;

  // update vertex length
  desc->vertex->length += vertex_count * VERTEX_STRIDE;
}

/**
   Add to vertices of specific color to the mesh vertex and indice array.
   Actually drawing a very very thin polygon so n points will:
   1. allocate 2n * vertex_stride in the vertex data
   2. 2n in the index
 */
void line_add_point(mesh *mesh, vec3 p1, vec3 p2, vec3 color) {

  if (mesh->vertex.length == LINE_MAX_POINTS - 3) {
    perror("Lines reached maximum, cannot add more point\n");
    return;
  }

  // extrude along each point normal
  vec3 dir;
  glm_vec3_sub(p2, p1, dir);

  vec3 up = {0.0f, 1.0f, 0.0f};
  if (fabsf(glm_vec3_dot(dir, up)) == 0.0f)
    glm_vec3_copy((vec3){1.0f, 0.0f, 0.0f}, up);

  // extrude base segment
  vec3 normal;
  glm_vec3_cross(p1, up, normal);
  glm_normalize(normal);
  glm_vec3_scale(normal, LINE_THICKNESS, normal);

  vec3 points_base[4];
  glm_vec3_add(p1, normal, points_base[3]);
  glm_vec3_add(p2, normal, points_base[2]);
  glm_vec3_sub(p1, normal, points_base[0]);
  glm_vec3_sub(p2, normal, points_base[1]);

  // add points to mesh vertex + index array
  line_create_plane(&(LineCreatePlaneDescriptor){
      .color = {color[0], color[1], color[2]},
      .index = &mesh->index,
      .vertex = &mesh->vertex,
      .points = points_base,
  });

  // create cross segment
  vec3 cross_normal;
  glm_vec3_cross(p1, normal, cross_normal);
  glm_normalize(cross_normal);
  glm_vec3_scale(cross_normal, LINE_THICKNESS, cross_normal);

  vec3 points_cross[4];
  glm_vec3_add(p1, cross_normal, points_cross[3]);
  glm_vec3_add(p2, cross_normal, points_cross[2]);
  glm_vec3_sub(p1, cross_normal, points_cross[0]);
  glm_vec3_sub(p2, cross_normal, points_cross[1]);

  // add points to mesh vertex + index array
  line_create_plane(&(LineCreatePlaneDescriptor){
      .color = {color[0], color[1], color[2]},
      .index = &mesh->index,
      .vertex = &mesh->vertex,
      .points = points_cross,
  });

  // update mesh vertex + index buffers
  mesh_set_vertex_attribute(mesh, &mesh->vertex);
  mesh_set_vertex_index(mesh, &mesh->index);
}
