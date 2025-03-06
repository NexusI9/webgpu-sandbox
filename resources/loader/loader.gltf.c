#include <stddef.h>
#include <stdint.h>
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

static void loader_gltf_add_vertex_attribute(vertex_attribute *, size_t,
                                             size_t);

static float *loader_gltf_attributes_from_accessor(cgltf_accessor *);
static void loader_gltf_accessor_to_array(cgltf_accessor *, float *, uint8_t);

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

void loader_gltf_add_vertex_attribute(vertex_attribute *attribute_list,
                                      size_t offset, size_t count) {}

float *loader_gltf_attributes_from_accessor(cgltf_accessor *accessor) {

  cgltf_buffer_view *buffer_view = accessor->buffer_view;

  // combine accessor and buffer view offset
  size_t offset = buffer_view->offset + accessor->offset;
  return (float *)((uint8_t *)buffer_view->buffer->data + offset);
}

static void loader_gltf_accessor_to_array(cgltf_accessor *accessor,
                                          float *destination, uint8_t count) {

  float *attributes = loader_gltf_attributes_from_accessor(accessor);
  size_t index = 0;
  for (size_t a = 0; a < accessor->count; a++) {
    for (uint8_t u = 0; u < count; u++) {
      // printf("%lu\t%lu\t%f\n", a, index, attributes[a * 3 + u]);
      destination[index++] = attributes[a * 3 + u];
    }
  }
}

void loader_gltf_create_mesh(mesh *mesh, cgltf_data *data) {
  // data->meshes;
  printf("load mesh\n");

  for (size_t m = 0; m < data->meshes_count; m++) {

    cgltf_mesh gl_mesh = data->meshes[m];
    vertex_attribute mesh_vert_attr;
    vertex_index mesh_vert_index;

    // get accessors to decode buffers into typed data (vertex, indices...)
    // load vertex attributes
    for (size_t a = 0; a < gl_mesh.primitives[0].attributes_count; a++) {

      cgltf_attribute *attribute = &gl_mesh.primitives[0].attributes[a];
      cgltf_accessor *accessor = gl_mesh.primitives[0].attributes[a].data;

      // fallback values in case no color or uv coordinates
      // need to maintain correct standaridzed structure for shaders
      const size_t vertex_number = accessor->count;
      float position[3 * vertex_number]; // vec3
      float normal[3 * vertex_number];   // vec3
      float color[3 * vertex_number];    // vec3
      float uv[2 * vertex_number];       // vec2
      size_t index[vertex_number];       // long

      // init to 0.0
      memset(position, 0, sizeof(position));
      memset(normal, 0, sizeof(normal));
      memset(color, 0, sizeof(color));
      memset(uv, 0, sizeof(uv));
      memset(index, 0, sizeof(index));

      switch (attribute->type) {

        // position
      case cgltf_attribute_type_position:
        loader_gltf_accessor_to_array(accessor, position, 3);
        break;

        // color
      case cgltf_attribute_type_color:
        loader_gltf_accessor_to_array(accessor, color, 3);
        break;

        // normals
      case cgltf_attribute_type_normal:
        loader_gltf_accessor_to_array(accessor, normal, 3);
        break;

        // uv
      case cgltf_attribute_type_texcoord:
        loader_gltf_accessor_to_array(accessor, uv, 2);
        break;

      default:
        break;
      }
    }

    // index
    cgltf_accessor *index_accessor = gl_mesh.primitives[0].indices;

    cgltf_buffer_view *index_buffer_view = index_accessor->buffer_view;
    size_t index_offset = index_buffer_view->offset + index_accessor->offset;

    printf("Index offset:%lu\n", index_offset);

    size_t *index_data =
        (size_t *)index_accessor->buffer_view->buffer->data + index_offset;

    printf("Index count: %zu, component_type: %d\n", index_accessor->count,
           index_accessor->component_type);

    for (size_t i = 0; i < index_accessor->count; i++) {
      printf("index %lu: %lu\n", i, index_data[i]);
    }
  }
}

void loader_gltf_create_shader(shader *shader, cgltf_data *data) {}
