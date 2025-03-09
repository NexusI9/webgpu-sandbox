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
static vertex_index loader_gltf_index(cgltf_primitive *);

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
    for (uint8_t x = 0; x < dimension; x++) {
      size_t index = x + i;
      // prevent overflow
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

vertex_index loader_gltf_index(cgltf_primitive *source) {

  // index
  cgltf_accessor *index_accessor = source->indices;
  cgltf_buffer_view *index_buffer_view = index_accessor->buffer_view;
  size_t index_offset = index_buffer_view->offset + index_accessor->offset;

  uint16_t *index_data =
      (uint16_t *)((uint8_t *)index_buffer_view->buffer->data + index_offset);

  // DELETEME: print_list_uint16(index_data, index_accessor->count, 1);

  return (vertex_index){.data = index_data, .length = index_accessor->count};
}

void loader_gltf_create_mesh(mesh *mesh, cgltf_data *data) {

  // data->meshes
  for (size_t m = 0; m < data->meshes_count; m++) {

    cgltf_mesh gl_mesh = data->meshes[m];
    struct mesh *parent_mesh = mesh;

    // if > 0 mesh, then append as child of initial mesh (level 1)
    if (m > 0) {
      size_t new_child = mesh_add_child_empty(mesh);
      parent_mesh = mesh_get_child(mesh, new_child);
    }

    // GLTF PRIMITIVES
    // primitives are vertices that belong to a same mesh but have different
    // material/shader.
    // In the current case we separate the primitives into mesh
    // children maybe in the future we will need to create a dedicated array.
    // primitive 0 = parent, primitive n = child
    for (size_t p = 0; p < gl_mesh.primitives_count; p++) {

      // get accessors to decode buffers into typed data (vertex, indices...)
      // load vertex attributes

      vertex_list vert_list; // raw vertex list (non-interleaved)

      vertex_attribute vert_attr;
      vertex_index vert_index;

      cgltf_primitive current_primitive = gl_mesh.primitives[p];

      // Initialize vertex lists with 0.0:
      // need fallback values in case no color or uv coordinates
      // ensure to maintain correct standaridzed structure for shaders
      loader_gltf_init_vertex_lists(
          &vert_attr, &vert_list, current_primitive.attributes[0].data->count);

      for (size_t a = 0; a < current_primitive.attributes_count; a++) {

        cgltf_attribute *attribute = &current_primitive.attributes[a];
        cgltf_accessor *accessor = current_primitive.attributes[a].data;

        switch (attribute->type) {

          // position
        case cgltf_attribute_type_position:
          loader_gltf_accessor_to_array(accessor, vert_list.position, 3);
          // interleave vertex data
          loader_gltf_add_vertex_attribute(&vert_attr, vert_list.position, 0,
                                           vert_list.count, 3);
          break;

          // normals
        case cgltf_attribute_type_normal:
          loader_gltf_accessor_to_array(accessor, vert_list.normal, 3);
          loader_gltf_add_vertex_attribute(&vert_attr, vert_list.normal, 3,
                                           vert_list.count, 3);
          break;

          // color
        case cgltf_attribute_type_color:
          loader_gltf_accessor_to_array(accessor, vert_list.color, 3);
          loader_gltf_add_vertex_attribute(&vert_attr, vert_list.color, 6,
                                           vert_list.count, 3);
          break;

          // uv
        case cgltf_attribute_type_texcoord:
          loader_gltf_accessor_to_array(accessor, vert_list.uv, 2);
          loader_gltf_add_vertex_attribute(&vert_attr, vert_list.uv, 9,
                                           vert_list.count, 2);
          break;

        default:
          break;
        }
      }

      // DELETEME: print_list_float(target_mesh.vertex.data,
      // target_mesh.vertex.length, VERTEX_STRIDE);

      // load index
      vert_index = loader_gltf_index(&current_primitive);

      // "primitive mesh" that will either be added as
      // parent or child depending on index
      if (p == 0) {
        parent_mesh->vertex = vert_attr;
        parent_mesh->index = vert_index;
      } else {
        // add child to parent mesh if current primitive > 0
        size_t new_child = mesh_add_child_empty(mesh);
        struct mesh *child_mesh = mesh_get_child(mesh, new_child);
        child_mesh->vertex = vert_attr;
        child_mesh->index = vert_index;
      }
    }
  }
}

void loader_gltf_create_shader(shader *shader, cgltf_data *data) {}
