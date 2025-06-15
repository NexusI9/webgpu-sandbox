#include "core.h"
#include "../runtime/material/material.h"
#include "../utils/system.h"
#include "string.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include <stdio.h>

static void line_create_plane(const LineCreatePlaneDescriptor *);

void line_create(Mesh *mesh, const LineCreateDescriptor *desc) {

  mesh_create(mesh, &(MeshCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .vertex = (VertexAttribute){0},
                        .index = (VertexIndex){0},
                        .name = desc->name,
                    });

  // create vertex attributes
  vattr_t *vertex_attributes = calloc(
      LINE_MAX_POINTS * VERTEX_STRIDE * LINE_VERTEX_COUNT, sizeof(vattr_t));

  mesh_topology_base_create_vertex_attribute(&mesh->topology.base,
                                             &(VertexAttribute){
                                                 .entries = vertex_attributes,
                                                 .length = 0,
                                             },
                                             desc->device, desc->queue);

  // crate vertex index
  vindex_t *vertex_index =
      calloc(LINE_MAX_POINTS * LINE_INDEX_COUNT, sizeof(vindex_t));

  mesh_topology_base_create_vertex_index(&mesh->topology.base,
                                         &(VertexIndex){
                                             .entries = vertex_index,
                                             .length = 0,
                                         },
                                         desc->device, desc->queue);

  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .path = "./runtime/assets/shader/shader.line.wgsl",
                            .label = "line",
                            .name = "line",
                            .device = desc->device,
                            .queue = desc->queue,
                        });

  material_texture_double_sided(mesh);
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

  desc->index->entries[desc->index->length] = (vindex_t)vertex_length;
  desc->index->entries[desc->index->length + 1] = (vindex_t)vertex_length + 1;
  desc->index->entries[desc->index->length + 2] = (vindex_t)vertex_length + 2;
  desc->index->entries[desc->index->length + 3] = (vindex_t)vertex_length + 2;
  desc->index->entries[desc->index->length + 4] = (vindex_t)vertex_length + 3;
  desc->index->entries[desc->index->length + 5] = (vindex_t)vertex_length;

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
    bool isB = p > 0 && p < 3;

    float *base = isB ? p2 : p1;
    float *opposite = isB ? p1 : p2;
    float dir_mul = isB ? 1.0f : -1.0f;

    float side = (p < 2) ? 1.0f : -1.0f;

    line_set_vertex(base, opposite, color, (vec2){side, dir_mul},
                    vertex_attribute->length, vertex_attribute->entries);

    vertex_attribute->length += VERTEX_STRIDE;
  }

  // update index array
  size_t vertex_length =
      (vertex_attribute->length / VERTEX_STRIDE) - LINE_VERTEX_COUNT;

  vertex_index->entries[vertex_index->length] = (vindex_t)vertex_length;
  vertex_index->entries[vertex_index->length + 1] = (vindex_t)vertex_length + 1;
  vertex_index->entries[vertex_index->length + 2] = (vindex_t)vertex_length + 2;
  vertex_index->entries[vertex_index->length + 3] = (vindex_t)vertex_length;
  vertex_index->entries[vertex_index->length + 4] = (vindex_t)vertex_length + 2;
  vertex_index->entries[vertex_index->length + 5] = (vindex_t)vertex_length + 3;

  vertex_index->length += 6;
}

void line_update_buffer(Mesh *mesh) {
  // update mesh vertex + index buffers
  mesh_topology_base_create_vertex_attribute(&mesh->topology.base,
                                             &mesh->topology.base.attribute,
                                             mesh->device, mesh->queue);

  mesh_topology_base_create_vertex_index(&mesh->topology.base,
                                         &mesh->topology.base.index,
                                         mesh->device, mesh->queue);
}
