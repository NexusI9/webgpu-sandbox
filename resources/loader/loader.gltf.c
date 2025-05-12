#include "loader.gltf.h"
#include "../geometry/vertex.h"
#include "../utils/system.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
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
#include "../runtime/material.h"
#include "../runtime/texture.h"
#include "stb/stb_image.h"

static void loader_gltf_create_mesh(scene *, WGPUDevice *, WGPUQueue *,
                                    cgltf_data *);
static void loader_gltf_create_shader(shader *, WGPUDevice *, WGPUQueue *,
                                      cgltf_primitive *);

static void loader_gltf_add_vertex_attribute(VertexAttribute *, float *, size_t,
                                             size_t, uint8_t);

static void loader_gltf_init_vertex_lists(VertexAttribute *, VertexList *,
                                          size_t);
static float *loader_gltf_attributes(cgltf_accessor *);
static void loader_gltf_accessor_to_array(cgltf_accessor *, float *, uint8_t);
static VertexIndex loader_gltf_index(cgltf_primitive *);
static void loader_gltf_bind_uniforms(shader *, cgltf_material *);
static uint8_t loader_gltf_extract_texture(cgltf_texture_view *,
                                           ShaderBindGroupTextureEntry *);
static void loader_gltf_load_fallback_texture(ShaderBindGroupTextureEntry *);
static void loader_gltf_mesh_position(mesh *, const char *, cgltf_data *);

void loader_gltf_load(const GLTFLoadDescriptor *desc) {

  cgltf_data *data = NULL;
  // load json structure
  cgltf_result result =
      cgltf_parse_file(desc->cgltf_options, desc->path, &data);

  // load actual gltf buffer data
  result = cgltf_load_buffers(desc->cgltf_options, data, desc->path);

  switch (result) {

  case cgltf_result_invalid_json:
    perror("Invalid GLTF JSON\n"), exit(1);
    break;

  case cgltf_result_success:
    loader_gltf_create_mesh(desc->scene, desc->device, desc->queue, data);
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

void loader_gltf_add_vertex_attribute(VertexAttribute *vert_attribute,
                                      float *data, size_t offset, size_t count,
                                      uint8_t dimension) {
  size_t row = 0;
  for (size_t i = 0; i < count * dimension; i += dimension) {
    size_t row_offset = row * VERTEX_STRIDE + offset;
    for (uint8_t x = 0; x < dimension; x++) {
      size_t index = x + i;
      // prevent overflow
      if (row_offset + x < vert_attribute->length)
        vert_attribute->entries[row_offset + x] = data[index];
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

void loader_gltf_init_vertex_lists(VertexAttribute *attributes,
                                   VertexList *list, size_t count) {

  // init vertex list
  vertex_list_create(list, count);

  // init vertex data (interleaved attributes)
  // list->count is the number of vertex used by index array
  // need to multiply by 3
  attributes->length = list->count * VERTEX_STRIDE;
  attributes->entries = (float *)calloc(attributes->length, sizeof(float));
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

VertexIndex loader_gltf_index(cgltf_primitive *source) {

  // index
  cgltf_accessor *index_accessor = source->indices;
  cgltf_buffer_view *index_buffer_view = index_accessor->buffer_view;
  size_t index_offset = index_buffer_view->offset + index_accessor->offset;

  uint16_t *index_data =
      (uint16_t *)((uint8_t *)index_buffer_view->buffer->data + index_offset);

  // DELETEME: print_list_uint16(index_data, index_accessor->count, 1);

  return (VertexIndex){.entries = index_data, .length = index_accessor->count};
}

void loader_gltf_create_mesh(scene *scene, WGPUDevice *device, WGPUQueue *queue,
                             cgltf_data *data) {

  printf("====== LOAD GLTF =====\n");

  // data->meshes
  for (size_t m = 0; m < data->meshes_count; m++) {

    cgltf_mesh gl_mesh = data->meshes[m];
    printf("Mesh name: %s\n", gl_mesh.name);

    struct mesh *scene_mesh = scene_new_mesh_lit(scene);
    mesh_create(scene_mesh, &(MeshCreateDescriptor){
                                .device = device,
                                .queue = queue,
                                .name = gl_mesh.name,
                                .vertex = (VertexAttribute){0},
                                .index = (VertexIndex){0},
                            });

    // set mesh position
    loader_gltf_mesh_position(scene_mesh, gl_mesh.name, data);

    /*
      GLTF PRIMITIVES
      primitives are vertices that belong to a same mesh but have different
      material/shader.
      In the current case we separate the primitives into mesh
      children maybe in the future we will need to create a dedicated array.
      primitive 0 = parent, primitive n = child
    */
    for (size_t p = 0; p < gl_mesh.primitives_count; p++) {
      // get accessors to decode buffers into typed data (vertex, indices...)
      // load vertex attributes

      VertexList vert_list; // raw vertex list (non-interleaved)
      VertexAttribute vert_attr;
      VertexIndex vert_index;

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

      // target current mesh itself if primitive == 0
      struct mesh *target_mesh = scene_mesh;

      // add child to parent mesh if current primitive > 0
      // and set it as target mesh
      if (p > 0) {
        printf("primitive %lu\n", p);
        target_mesh = scene_new_mesh_lit(scene);

        // add target mesh pointer to parent mesh children list
        mesh_add_child(target_mesh, scene_mesh);

        /*
          need to dynamically allocate name
           iteration use same frame stack
           meaning addresses will be reused throughout the loop
           this leads the latest mesh name (pointer) -
           to be shared accross all children mesh
           (same issue with shader)
        */

        char *mesh_name;
        asprintf(&mesh_name, "%s %lu", gl_mesh.name, p);
        mesh_create(target_mesh, &(MeshCreateDescriptor){
                                     .device = device,
                                     .queue = queue,
                                     .name = mesh_name,
                                     .vertex = (VertexAttribute){0},
                                     .index = (VertexIndex){0},
                                 });
      }

      // load shader
      loader_gltf_create_shader(mesh_shader_texture(target_mesh), device, queue,
                                &current_primitive);

      // dynamically define mesh attribute
      mesh_set_vertex_attribute(target_mesh, &vert_attr);
      mesh_set_vertex_index(target_mesh, &vert_index);

      material_texture_bind_views(target_mesh, &scene->camera, &scene->viewport,
                                  1);
      // TODO: put the texture bind lights to the scene itself
      material_texture_bind_lights(target_mesh, &scene->lights.ambient,
                                   &scene->lights.spot, &scene->lights.point,
                                   &scene->lights.sun, 2);
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

  loader_gltf_bind_uniforms(shader, material);
}

/**
  Bind PBR textures
  store the texture_views (hold pointer to actual texture + other data)
 */
void loader_gltf_bind_uniforms(shader *shader, cgltf_material *material) {

  uint8_t texture_length = 4;

  // TODO: check how to handle if object already has a AO Texture imported ?
  // overwrite ?
  //&material->occlusion_texture : baked separately in the AO pass,
  cgltf_texture_view *texture_view_list[] = {
      &material->pbr_metallic_roughness.base_color_texture,
      &material->pbr_metallic_roughness.metallic_roughness_texture,
      &material->normal_texture,
      &material->emissive_texture,
  };

  ShaderBindGroupTextureEntry texture_entries[texture_length];
  ShaderBindGroupSamplerEntry sampler_entries[texture_length];

  uint8_t binding = 0;
  for (int t = 0; t < texture_length; t++) {

    loader_gltf_extract_texture(texture_view_list[t], &texture_entries[t]);

    // create texture entries
    texture_entries[t] = (ShaderBindGroupTextureEntry){
        .binding = binding,
        .data = texture_entries[t].data,
        .size = texture_entries[t].size,
        .width = texture_entries[t].width,
        .height = texture_entries[t].height,
        .dimension = WGPUTextureViewDimension_2D,
        .format = WGPUTextureFormat_BGRA8Unorm,
        .channels = TEXTURE_CHANNELS_RGBA,
        .sample_type = WGPUTextureSampleType_Float,
    };

    // create sampler entries
    sampler_entries[t] = (ShaderBindGroupSamplerEntry){
        .binding = binding + 1,
        .type = WGPUSamplerBindingType_Filtering,
        .addressModeU = WGPUAddressMode_ClampToEdge,
        .addressModeV = WGPUAddressMode_ClampToEdge,
        .addressModeW = WGPUAddressMode_ClampToEdge,
        .minFilter = WGPUFilterMode_Linear,
        .magFilter = WGPUFilterMode_Linear,
        .compare = WGPUCompareFunction_Undefined,
    };

    binding += 2;
  }

  // send texture + sampler to shader
  shader_add_texture(shader, &(ShaderCreateTextureDescriptor){
                                 .group_index = 0,
                                 .entry_count = texture_length,
                                 .entries = texture_entries,
                                 .visibility = WGPUShaderStage_Fragment,
                             });

  shader_add_sampler(shader, &(ShaderCreateSamplerDescriptor){
                                 .group_index = 0,
                                 .entry_count = texture_length,
                                 .entries = sampler_entries,
                                 .visibility = WGPUShaderStage_Fragment,
                             });
}

uint8_t loader_gltf_extract_texture(cgltf_texture_view *texture_view,
                                    ShaderBindGroupTextureEntry *shader_entry) {

  if (texture_view->texture) {
    // extract textures from texture_view
    // 1. if uri => load image (TODO)
    // 2. if buffer_view => store buffer & size

    // TODO: check why cgltf buffer->size return smaller size that w * h *
    // channels
    cgltf_image *image = texture_view->texture->image;
    int width, height, channels;
    if (image->buffer_view) {
      cgltf_decode_uri(image->uri);

      unsigned char *gltf_data =
          (unsigned char *)image->buffer_view->buffer->data +
          image->buffer_view->offset;

      // use stbi to convert gltf image from RGB(A) to RGBA, ensuring 4 channels
      // TODO: more flexible texture upload (RGB/RGBA, large texture
      // handling...)
      shader_entry->data = stbi_load_from_memory(
          gltf_data, image->buffer_view->buffer->size, &shader_entry->width,
          &shader_entry->height, &channels, TEXTURE_CHANNELS_RGBA);

      shader_entry->size =
          shader_entry->width * shader_entry->height * TEXTURE_CHANNELS_RGBA;

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

  return 0;
}

void loader_gltf_load_fallback_texture(
    ShaderBindGroupTextureEntry *shader_entry) {

  shader_entry->width = TEXTURE_MIN_SIZE;
  shader_entry->height = TEXTURE_MIN_SIZE;

  texture_create_by_ref(&shader_entry->data, &shader_entry->size,
                        &(TextureCreateDescriptor){
                            .width = shader_entry->width,
                            .height = shader_entry->height,
                            .value = 255,
                            .channels = TEXTURE_CHANNELS_RGBA,
                        });
}

void loader_gltf_mesh_position(mesh *mesh, const char *name, cgltf_data *data) {

  // Apply transformation to mesh
  // Transformation attributes are stored in the nodes
  // whereas mesh only contain vertices/index related data
  // need to go through the nodes and compare with the given gltf_mesh to see if
  // it matches name

  for (size_t n = 0; n < data->nodes_count; n++) {

    cgltf_node *node = &data->nodes[n];
    if (strcmp(node->mesh->name, name) == 0) {

      // set translation
      if (node->has_translation)
        mesh_position(mesh, (vec3){
                                node->translation[0],
                                node->translation[1],
                                node->translation[2],
                            });

      // set scale
      if (node->has_scale)
        mesh_scale(mesh, (vec3){
                             node->scale[0],
                             node->scale[1],
                             node->scale[2],
                         });
      // set rotation
      if (node->has_rotation)
        mesh_rotate_quat(mesh, node->rotation);
    }
  }
}
