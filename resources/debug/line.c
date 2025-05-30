#include "line.h"
#include "../utils/system.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include <stdio.h>

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
          .entries = calloc(LINE_MAX_POINTS * VERTEX_STRIDE * LINE_VERTEX_COUNT,
                            sizeof(float)),
          .length = 0,
      });

  mesh_set_vertex_index(
      mesh, &(VertexIndex){
                .entries = calloc(LINE_MAX_POINTS * LINE_INDEX_COUNT,
                                  sizeof(uint16_t)),
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

/** Define vertex data from a vertex array.
    Use different structure as casual vertex:

    Position => Position A
    Normal   => Position Ā
    Color    => Color
    UV       => [0] Extrude Direction, [1] Thickness
 */
void line_set_vertex(const vec3 base, const vec3 opposite, const vec4 color,
                     const vec2 extra, const size_t offset, float *data) {

  // build up a new vertex with null normals and uv, but valid colors and
  // position

  // set position
  data[offset + 0] = base[0];
  data[offset + 1] = base[1];
  data[offset + 2] = base[2];

  // set normal
  data[offset + 3] = opposite[0];
  data[offset + 4] = opposite[1];
  data[offset + 5] = opposite[2];

  // set color
  data[offset + 6] = color[0];
  data[offset + 7] = color[1];
  data[offset + 8] = color[2];

  // set UV
  data[offset + 9] = extra[0];
  data[offset + 10] = extra[1];
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
  line_set_vertex(desc->points[0], normal, desc->color, uv, vertex_offset,
                  desc->vertex->entries);

  // B (p1 thickness)
  line_set_vertex(desc->points[1], normal, desc->color, uv,
                  vertex_offset + VERTEX_STRIDE, desc->vertex->entries);

  // C (p2 thickness)
  line_set_vertex(desc->points[2], normal, desc->color, uv,
                  vertex_offset + 2 * VERTEX_STRIDE, desc->vertex->entries);

  // D (p2)
  line_set_vertex(desc->points[3], normal, desc->color, uv,
                  vertex_offset + 3 * VERTEX_STRIDE, desc->vertex->entries);

  // add indices (A-B-C & A-C-D)
  size_t vertex_length = desc->vertex->length / VERTEX_STRIDE;

  desc->index->entries[desc->index->length] = (uint16_t)vertex_length;
  desc->index->entries[desc->index->length + 1] = (uint16_t)vertex_length + 1;
  desc->index->entries[desc->index->length + 2] = (uint16_t)vertex_length + 2;
  desc->index->entries[desc->index->length + 3] = (uint16_t)vertex_length + 2;
  desc->index->entries[desc->index->length + 4] = (uint16_t)vertex_length + 3;
  desc->index->entries[desc->index->length + 5] = (uint16_t)vertex_length;

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
void line_add_point(vec3 p1, vec3 p2, vec3 color,
                    VertexAttribute *vertex_attribute,
                    VertexIndex *vertex_index) {

  if (vertex_attribute->length / VERTEX_STRIDE / LINE_VERTEX_COUNT ==
      LINE_MAX_POINTS - LINE_VERTEX_COUNT - 1) {
    // perror("Lines reached maximum, cannot add more point\n");
    return;
  }

  // update vertex array
  for (int p = 0; p < LINE_VERTEX_COUNT; p++) {
    float *base = (p % 2 == 0) ? p1 : p2;
    float *opposite = (p % 2 == 0) ? p2 : p1;
    float side = (p <= 1) ? -1.0f : 1.0f;

    line_set_vertex(base, opposite, color, (vec2){side, LINE_THICKNESS},
                    vertex_attribute->length, vertex_attribute->entries);

    vertex_attribute->length += VERTEX_STRIDE;
  }

  // updat index array
  size_t vertex_length =
      (vertex_attribute->length / VERTEX_STRIDE) - LINE_VERTEX_COUNT;

  vertex_index->entries[vertex_index->length] = (uint16_t)vertex_length;
  vertex_index->entries[vertex_index->length + 1] = (uint16_t)vertex_length + 1;
  vertex_index->entries[vertex_index->length + 2] = (uint16_t)vertex_length + 2;
  vertex_index->entries[vertex_index->length + 3] = (uint16_t)vertex_length + 2;
  vertex_index->entries[vertex_index->length + 4] = (uint16_t)vertex_length + 3;
  vertex_index->entries[vertex_index->length + 5] = (uint16_t)vertex_length + 1;

  vertex_index->length += 6;
}

void line_update_buffer(mesh *mesh) {
  // update mesh vertex + index buffers
  mesh_set_vertex_attribute(mesh, &mesh->vertex.base.attribute);
  mesh_set_vertex_index(mesh, &mesh->vertex.base.index);
}
