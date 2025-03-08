#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define CGLTF_IMPLEMENTATION
#include "../include/cglm/vec2.h"
#include "../include/cglm/vec3.h"
#include "../include/cglm/vec4.h"
#include "../runtime/vertex.h"
#include "../utils/system.h"
#include "loader.gltf.h"
#include <stdio.h>

static void loader_gltf_create_mesh(mesh *, cgltf_data *);
static void loader_gltf_create_shader(shader *, cgltf_data *);

static void loader_gltf_add_vertex_attribute(vertex_attribute *, float *,
                                             size_t, size_t, uint8_t);

static void loader_gltf_init_vertex_lists(vertex_attribute *, vertex_list *,
                                          size_t);
static float *loader_gltf_attributes(cgltf_accessor *);
static void loader_gltf_accessor_to_array(cgltf_accessor *, float *, uint8_t);
static uint16_t *loader_gltf_index(cgltf_mesh *);

void loader_gltf_load(mesh *mesh, const char *path,
                      const cgltf_options *options) {

  cgltf_data *data = NULL;
  // load json structure
  cgltf_result result = cgltf_parse_file(options, path, &data);

  // load actual gltf buffer data
  result = cgltf_load_buffers(options, data, path);

  switch (result) {

  case cgltf_result_invalid_json:
    perror("Invalid GLTF JSON\n"), exit(1);
    break;

  case cgltf_result_success:
    loader_gltf_create_mesh(mesh, data);
    break;

  case cgltf_result_file_not_found:
    perror("GLTF file not found\n"), exit(1);
    break;

  case cgltf_result_out_of_memory:
    perror("GLTF loading aborted, out of memory\n"), exit(1);
    break;

  default:
    perror("GLTF loading aborted, unhanded error\n"), exit(1);
    break;
  }

  cgltf_free(data);
}

void loader_gltf_add_vertex_attribute(vertex_attribute *vert_attribute,
                                      float *data, size_t offset, size_t count,
                                      uint8_t dimension) {
  size_t row = 0;
  for (size_t i = 0; i < count * dimension; i += dimension) {
    size_t row_offset = row * VERTEX_STRIDE + offset;
    // prevent overflow
    for (uint8_t x = 0; x < dimension; x++) {
      size_t index = x + i;
      if (row_offset + x < vert_attribute->length)
        vert_attribute->data[row_offset + x] = data[index];
    }
    row++;
  }

}

float *loader_gltf_attributes(cgltf_accessor *accessor) {

  cgltf_buffer_view *buffer_view = accessor->buffer_view;

  // combine accessor and buffer view offset
  size_t offset = buffer_view->offset + accessor->offset;
  return (float *)((uint8_t *)buffer_view->buffer->data + offset);
}

void loader_gltf_init_vertex_lists(vertex_attribute *attributes,
                                   vertex_list *list, size_t count) {

  // init vertex list
  vertex_list_create(list, count);

  // init vertex data (interleaved attributes)
  // list->count is the number of vertex used by index array
  // need to multiply by 3
  attributes->length = list->count * VERTEX_STRIDE;
  attributes->data = (float *)calloc(attributes->length, sizeof(float));
}

static void loader_gltf_accessor_to_array(cgltf_accessor *accessor,
                                          float *destination, uint8_t count) {

  float *attributes = loader_gltf_attributes(accessor);
  size_t index = 0;
  for (size_t a = 0; a < accessor->count; a++) {
    for (uint8_t u = 0; u < count; u++) {
      destination[index++] = attributes[a * count + u];
    }
  }
}

uint16_t *loader_gltf_index(cgltf_mesh *mesh) {

  // index
  cgltf_accessor *index_accessor = mesh->primitives[0].indices;
  cgltf_buffer_view *index_buffer_view = index_accessor->buffer_view;
  size_t index_offset = index_buffer_view->offset + index_accessor->offset;

  uint16_t *index_data =
      (uint16_t *)((uint8_t *)index_buffer_view->buffer->data + index_offset);

  //DELETEME: print_list_uint16(index_data, index_accessor->count, 1);

  return index_data;
}

void loader_gltf_create_mesh(mesh *mesh, cgltf_data *data) {
  // data->meshes
  for (size_t m = 0; m < data->meshes_count; m++) {

    cgltf_mesh gl_mesh = data->meshes[m];

    vertex_list vert_list;
    vertex_attribute vert_attr;
    vertex_index vert_index;

    // get accessors to decode buffers into typed data (vertex, indices...)
    // load vertex attributes
    for (size_t a = 0; a < gl_mesh.primitives[0].attributes_count; a++) {

      cgltf_attribute *attribute = &gl_mesh.primitives[0].attributes[a];
      cgltf_accessor *accessor = gl_mesh.primitives[0].attributes[a].data;

      // init on 0
      // fallback values in case no color or uv coordinates
      // need to maintain correct standaridzed structure for shaders
      if (a == 0)
        loader_gltf_init_vertex_lists(&vert_attr, &vert_list, accessor->count);

      printf("accessor:%lu\n", accessor->count);
      switch (attribute->type) {

        // position
      case cgltf_attribute_type_position:
        printf("position\n");
        loader_gltf_accessor_to_array(accessor, vert_list.position, 3);
        // interleave vertex data
        loader_gltf_add_vertex_attribute(&vert_attr, vert_list.position, 0,
                                         vert_list.count, 3);
        break;

        // normals
      case cgltf_attribute_type_normal:
        printf("normal\n");
        loader_gltf_accessor_to_array(accessor, vert_list.normal, 3);
        loader_gltf_add_vertex_attribute(&vert_attr, vert_list.normal, 3,
                                         vert_list.count, 3);
        break;

        // color
      case cgltf_attribute_type_color:
        printf("color\n");
        loader_gltf_accessor_to_array(accessor, vert_list.color, 3);
        loader_gltf_add_vertex_attribute(&vert_attr, vert_list.color, 6,
                                         vert_list.count, 3);
        break;

        // uv
      case cgltf_attribute_type_texcoord:
        printf("uv\n");
        loader_gltf_accessor_to_array(accessor, vert_list.uv, 2);
        loader_gltf_add_vertex_attribute(&vert_attr, vert_list.uv, 9,
                                         vert_list.count, 2);
        break;

      default:
        break;
      }
    }

    //DELETEME: print_list_float(vert_attr.data, vert_attr.length, VERTEX_STRIDE);

    // load index
    vert_index.data = loader_gltf_index(&gl_mesh);
  }
}

void loader_gltf_create_shader(shader *shader, cgltf_data *data) {}
