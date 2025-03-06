#include <stddef.h>
#include <stdint.h>
#define CGLTF_IMPLEMENTATION
#include "../include/cglm/vec2.h"
#include "../include/cglm/vec3.h"
#include "../include/cglm/vec4.h"
#include "loader.gltf.h"
#include <stdio.h>

static void loader_gltf_create_mesh(mesh *, cgltf_data *);
static void loader_gltf_create_shader(shader *, cgltf_data *);

static void loader_gltf_add_vertex_attribute(vertex_attribute *, size_t,
                                             size_t);

static void loader_gltf_accessor_to_vec4(cgltf_accessor *, vec4);
static void loader_gltf_accessor_to_vec3(cgltf_accessor *, vec3);
static void loader_gltf_accessor_to_vec2(cgltf_accessor *, vec2);

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

static void loader_gltf_accessor_to_vec3(cgltf_accessor *accessor,
                                         vec3 destination) {

  cgltf_buffer_view *buffer_view = accessor->buffer_view;

  // combine accessor and buffer view offset
  size_t offset = buffer_view->offset + accessor->offset;
  float *attributes = (float *)((uint8_t *)buffer_view->buffer->data + offset);

  for (size_t a = 0; a < accessor->count; a++) {
    printf("attribute %lu: %f, %f, %f\n", a, attributes[a * 3],
           attributes[a * 3 + 1], attributes[a * 3 + 2]);
  }
}

static void loader_gltf_accessor_to_vec2(cgltf_accessor *accessor,
                                         vec2 destination) {}

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
      
      vec3 position = {0.0f, 0.0f, 0.0f};
      vec3 normal = {0.0f, 0.0f, 0.0f};
      vec3 color = {0.0f, 0.0f, 0.0f};
      vec2 uv = {0.0f, 0.0f};
      switch (attribute->type) {

        // position
      case cgltf_attribute_type_position:

        break;

        // color
      case cgltf_attribute_type_color:

        break;

        // normals
      case cgltf_attribute_type_normal:

        break;

        // uv
      case cgltf_attribute_type_texcoord:

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
