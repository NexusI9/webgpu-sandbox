#include "loader.gltf.h"
#include "../include/cglm/vec2.h"
#include "../include/cglm/vec3.h"
#include "../include/cglm/vec4.h"
#include "../runtime/vertex.h"
#include "../utils/system.h"
#include "webgpu/webgpu.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"
#include "limits.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../backend/buffer.h"
#include "stb/stb_image.h"

static void loader_gltf_create_mesh(mesh *, cgltf_data *);
static void loader_gltf_create_shader(shader *, WGPUDevice *, WGPUQueue *,
                                      cgltf_primitive *);

static void loader_gltf_add_vertex_attribute(vertex_attribute *, float *,
                                             size_t, size_t, uint8_t);

static void loader_gltf_init_vertex_lists(vertex_attribute *, vertex_list *,
                                          size_t);
static float *loader_gltf_attributes(cgltf_accessor *);
static void loader_gltf_accessor_to_array(cgltf_accessor *, float *, uint8_t);
static vertex_index loader_gltf_index(cgltf_primitive *);
static void loader_gltf_bind_uniforms(shader *, cgltf_material *);
static uint8_t loader_gltf_extract_texture(cgltf_texture_view *,
                                           ShaderBindGroupTextureEntry *);
static void loader_gltf_load_fallback_texture(ShaderBindGroupTextureEntry *);

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

    // shader *shader_list[gl_mesh.primitives_count];

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

      // DELETEME: print_list_float(vert_attr.data,
      // vert_attr.length, VERTEX_STRIDE);

      // load index
      vert_index = loader_gltf_index(&current_primitive);

      // add to parent itself if primitive == 0
      struct mesh *target_mesh = parent_mesh;

      // add child to parent mesh if current primitive > 0
      if (p > 0) {
        size_t new_child = mesh_add_child_empty(parent_mesh);
        struct mesh *child_mesh = mesh_get_child(parent_mesh, new_child);
        target_mesh = child_mesh;

        // need to dynamically allocate name
        // iteration use same frame stack
        // meaning addresses will be reused throughout the loop
        // this leads the latest mesh name (pointer) -
        // to be shared accross all children mesh
        // (same issue with shader)

        asprintf(&target_mesh->name, "%s %lu", gl_mesh.name, p);
      } else {
        asprintf(&target_mesh->name, "%s", gl_mesh.name);
      }

      // load shader
      loader_gltf_create_shader(&target_mesh->shader, mesh->device, mesh->queue,
                                &current_primitive);

      // dynamically define mesh attribute
      mesh_set_vertex_attribute(target_mesh, &vert_attr);
      mesh_set_vertex_index(target_mesh, &vert_index);
      mesh_set_parent(target_mesh, parent_mesh);
    }
  }
}

void loader_gltf_create_shader(shader *shader, WGPUDevice *device,
                               WGPUQueue *queue, cgltf_primitive *primitive) {

  // Use default pbr shader as default
  // TODO: Add a custom path for different shader in loader configuration

  cgltf_material *material = primitive->material;
  shader_create(shader, &(ShaderCreateDescriptor){
                            .path = "./runtime/assets/shader/shader.pbr.wgsl",
                            .label = material->name,
                            .name = material->name,
                            .device = device,
                            .queue = queue,
                        });

  // TODO: bind pbr related uniforms
  loader_gltf_bind_uniforms(shader, material);
}

void loader_gltf_bind_uniforms(shader *shader, cgltf_material *material) {

  // 1. bind pbr factor
  ShaderPBRUniform uPBR = {
      .metallic_factor = material->pbr_metallic_roughness.metallic_factor,
      .roughness_factor = material->pbr_metallic_roughness.roughness_factor,
      .occlusion_factor = 1.0f,
      .normal_scale = 1.0f};

  glm_vec4_copy(material->pbr_metallic_roughness.base_color_factor,
                uPBR.diffuse_factor);

  glm_vec3_copy(material->emissive_factor, uPBR.emissive_factor);

  ShaderBindGroupEntry entries[1] = {{
      .binding = 0,
      .offset = 0,
      .size = sizeof(ShaderPBRUniform),
      .data = (void *)&uPBR,
  }};

  shader_add_uniform(shader, &(ShaderCreateUniformDescriptor){
                                 .group_index = 0,
                                 .entry_count = 1,
                                 .entries = entries,
                                 .visibility = WGPUShaderStage_Vertex |
                                               WGPUShaderStage_Fragment,
                             });

  // 2. bind pbdr textures
  // store the texture_views (hold pointer to actual texture + other data)
  ShaderPBRTextures shader_texture;
  uint8_t texture_length = 5;
  ShaderBindGroupTextureEntry *texture_list[] = {
      &shader_texture.diffuse,  &shader_texture.metallic,
      &shader_texture.normal,   &shader_texture.occlusion,
      &shader_texture.emissive,
  };

  cgltf_texture_view *texture_view_list[] = {
      &material->pbr_metallic_roughness.base_color_texture,
      &material->pbr_metallic_roughness.metallic_roughness_texture,
      &material->normal_texture,
      &material->occlusion_texture,
      &material->emissive_texture,
  };

  ShaderBindGroupTextureEntry texture_entries[texture_length];

  for (int t = 0; t < texture_length; t++) {
    loader_gltf_extract_texture(texture_view_list[t], texture_list[t]);
    texture_entries[t] = (ShaderBindGroupTextureEntry){
        .binding = t + 1,
        .data = texture_list[t]->data,
        .size = texture_list[t]->size,
        .width = texture_list[t]->width,
        .height = texture_list[t]->height,
    };
  }

  shader_add_texture(shader, &(ShaderCreateTextureDescriptor){
                                 .group_index = 0,
                                 .entry_count = texture_length,
                                 .entries = texture_entries,
                             });
}

uint8_t loader_gltf_extract_texture(cgltf_texture_view *texture_view,
                                    ShaderBindGroupTextureEntry *shader_entry) {

  if (texture_view->texture) {

    // extract textures from texture_view
    // 1. if uri => load image (TODO)
    // 2. if buffer_view => store buffer & size
    cgltf_image *image = texture_view->texture->image;
    int width, height, channels;
    if (image->buffer_view) {
      cgltf_decode_uri(image->uri);
      shader_entry->size = image->buffer_view->buffer->size;
      shader_entry->data = (unsigned char *)image->buffer_view->buffer->data +
                           image->buffer_view->offset;

      int stb_data = stbi_info_from_memory(
          shader_entry->data, shader_entry->size, &shader_entry->width,
          &shader_entry->height, &channels);

      return 1;

    } else {
      VERBOSE_PRINT(
          "Loader GLTF: Texture found but couldn't be loaded, loading "
          "default texture\n");
      loader_gltf_load_fallback_texture(shader_entry);
      return 0;
    }

  } else {
    VERBOSE_PRINT(
        "Loader GLTF: Couldn't find texture, loading default texture\n");
    loader_gltf_load_fallback_texture(shader_entry);
    return 0;
  }
}

void loader_gltf_load_fallback_texture(
    ShaderBindGroupTextureEntry *shader_entry) {
  uint8_t black_pixel[4] = {0, 0, 0, 255};

  shader_entry->data = black_pixel;
  shader_entry->width = 1;
  shader_entry->height = 1;
  shader_entry->size = sizeof(black_pixel);
}
