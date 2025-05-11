#include "line.h"
#include "../utils/system.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include <stdio.h>

static void line_push_vertex(const vec3, const vec3, const vec3, const vec2,
                             const size_t, float *);
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
      mesh,
      &(VertexAttribute){
          .data = calloc(LINE_MAX_POINTS * VERTEX_STRIDE * LINE_VERTEX_COUNT,
                         sizeof(float)),
          .length = 0,
      });

  mesh_set_vertex_index(
      mesh,
      &(VertexIndex){
          .data = calloc(LINE_MAX_POINTS * LINE_INDEX_COUNT, sizeof(uint16_t)),
          .length = 0,
      });

  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .path = "./runtime/assets/shader/shader.line.wgsl",
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

void line_push_vertex(const vec3 position, const vec3 normal, const vec4 color,
                      const vec2 uv, const size_t offset, float *data) {

  // build up a new vertex with null normals and uv, but valid colors and
  // position

  // set position
  data[offset + 0] = position[0];
  data[offset + 1] = position[1];
  data[offset + 2] = position[2];

  // set normal
  data[offset + 3] = normal[0];
  data[offset + 4] = normal[1];
  data[offset + 5] = normal[2];

  // set color
  data[offset + 6] = color[0];
  data[offset + 7] = color[1];
  data[offset + 8] = color[2];

  // set UV
  data[offset + 9] = uv[0];
  data[offset + 10] = uv[1];
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
  vec3 normal = {0.0f, 1.0f, 0.0f};
  vec2 uv = {0.0f, 0.0f};
  // add points to the array contiguously

  // A (p1)
  line_push_vertex(desc->points[0], normal, desc->color, uv, vertex_offset,
                   desc->vertex->data);

  // B (p1 thickness)
  line_push_vertex(desc->points[1], normal, desc->color, uv,
                   vertex_offset + VERTEX_STRIDE, desc->vertex->data);

  // C (p2 thickness)
  line_push_vertex(desc->points[2], normal, desc->color, uv,
                   vertex_offset + 2 * VERTEX_STRIDE, desc->vertex->data);

  // D (p2)
  line_push_vertex(desc->points[3], normal, desc->color, uv,
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

  if (mesh->vertex.length / VERTEX_STRIDE / LINE_VERTEX_COUNT ==
      LINE_MAX_POINTS - LINE_VERTEX_COUNT - 1) {
    // perror("Lines reached maximum, cannot add more point\n");
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

  const int vertex_count = 4;
  vec3 points_base[vertex_count];
  glm_vec3_copy((vec3){1.0f, 0.0f, 1.0f}, points_base[0]);
  glm_vec3_copy((vec3){-1.0f, 0.0f, 1.0f}, points_base[1]);
  glm_vec3_copy((vec3){-1.0f, 0.0f, -1.0f}, points_base[2]);
  glm_vec3_copy((vec3){1.0f, 0.0f, -1.0f}, points_base[3]);

  /*update vertex array
    Use different structure as casual vertex:

    Position => Position A
    Normal   => Position Ā
    Color    => Color
    UV       => [0] Extrude Direction, [1] Thickness

  */
  for (int p = 0; p < vertex_count; p++) {
    float *base = (p % 2 == 0) ? p1 : p2;
    float *opposite = (p % 2 == 0) ? p2 : p1;
    float side = (p <= 1) ? -1.0f : 1.0f;

    line_push_vertex(base, opposite, color, (vec2){side, LINE_THICKNESS},
                     mesh->vertex.length, mesh->vertex.data);

    mesh->vertex.length += VERTEX_STRIDE;
  }

  // updat index array
  size_t vertex_length =
      (mesh->vertex.length / VERTEX_STRIDE) - LINE_VERTEX_COUNT;

  mesh->index.data[mesh->index.length] = (uint16_t)vertex_length;
  mesh->index.data[mesh->index.length + 1] = (uint16_t)vertex_length + 1;
  mesh->index.data[mesh->index.length + 2] = (uint16_t)vertex_length + 2;
  mesh->index.data[mesh->index.length + 3] = (uint16_t)vertex_length + 2;
  mesh->index.data[mesh->index.length + 4] = (uint16_t)vertex_length + 3;
  mesh->index.data[mesh->index.length + 5] = (uint16_t)vertex_length + 1;

  mesh->index.length += 6;
}

void line_update_buffer(mesh *mesh) {
  // update mesh vertex + index buffers
  mesh_set_vertex_attribute(mesh, &mesh->vertex);
  mesh_set_vertex_index(mesh, &mesh->index);
}
