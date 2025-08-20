#include "loader.gltf.h"
#include "../backend/renderer/scene/std_texture/std_texture.h"
#include "../backend/renderer/scene/texture.h"
#include "webgpu/webgpu.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"

// gltf utils
static float *loader_gltf_attributes(cgltf_accessor *);
static void loader_gltf_accessor_to_array(cgltf_accessor *, float *, uint8_t);
static VertexIndex loader_gltf_index(cgltf_primitive *);

// vertex buffer utils
static void loader_gltf_add_vertex_attribute(VertexAttribute *, float *, size_t,
                                             size_t, uint8_t);

static void loader_gltf_init_vertex_lists(VertexAttribute *, VertexList *,
                                          size_t);
// mesh utils
static void loader_gltf_create_mesh(Scene *, const WGPUDevice, const WGPUQueue,
                                    cgltf_data *, const LoaderGLTFOptions *);
static void loader_gltf_mesh_position(Mesh *, const char *, cgltf_data *);

// shader utils

static void loader_gltf_bind_uniforms(Shader *, cgltf_material *,
                                      const LoaderGLTFOptions *);

static LoaderGLTFStatus loader_gltf_extract_texture(cgltf_texture_view *,
                                                    void **, size_t *, int *,
                                                    int *, int *,
                                                    TextureResolution);

void loader_gltf_load(const GLTFLoadDescriptor *desc) {

  VERBOSE_IMPORT("GLTF file: %s", desc->path);

  cgltf_data *data = NULL;
  // load json structure
  cgltf_result result =
      cgltf_parse_file(desc->cgltf_options, desc->path, &data);

  // load actual gltf buffer data
  result = cgltf_load_buffers(desc->cgltf_options, data, desc->path);

  switch (result) {

  case cgltf_result_invalid_json:
    VERBOSE_ERROR("Invalid GLTF JSON.");
    exit(1);
    break;

  case cgltf_result_success:
    loader_gltf_create_mesh(desc->scene, desc->device, desc->queue, data,
                            desc->options);
    break;

  case cgltf_result_file_not_found:
    VERBOSE_ERROR("GLTF file not found.");
    exit(1);
    break;

  case cgltf_result_out_of_memory:
    VERBOSE_ERROR("GLTF loading aborted, out of memory.");
    exit(1);
    break;

  default:
    VERBOSE_ERROR("GLTF loading aborted, unhanded error.");
    exit(1);
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
  attributes->capacity = attributes->length;
  attributes->entries = (float *)calloc(attributes->capacity, sizeof(float));
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
  size_t index_count = index_accessor->count;
  uint16_t *raw_index_data =
      (uint16_t *)((uint8_t *)index_buffer_view->buffer->data + index_offset);

  // gltf provide index a uint16, however the engine sur uint32, need to
  // manually cast it
  // alloc on heap cause VLA throws stack overflow
  vindex_t *index_data = malloc(sizeof(vindex_t) * index_count);
  for (size_t i = 0; i < index_count; i++)
    index_data[i] = (vindex_t)raw_index_data[i];

  // DELETEME: print_list_uint32(index_data, index_count, 1);
  return (VertexIndex){
      .entries = index_data,
      .capacity = index_count,
      .length = index_count,
  };
}

void loader_gltf_create_mesh(Scene *scene, const WGPUDevice device,
                             const WGPUQueue queue, cgltf_data *data,
                             const LoaderGLTFOptions *options) {

  // data->meshes
  for (size_t m = 0; m < data->meshes_count; m++) {

    cgltf_mesh gl_mesh = data->meshes[m];

    struct Mesh *scene_mesh = scene_new_mesh(scene);
    mesh_create(scene_mesh, &(MeshCreateDescriptor){
                                .device = device,
                                .queue = queue,
                                .name = gl_mesh.name,
                                .vertex = (VertexAttribute){0},
                                .index = (VertexIndex){0},
                            });

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
      VertexAttribute vert_attr = {0};
      VertexIndex vert_index = {0};

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
      struct Mesh *target_mesh = scene_mesh;

      // add child to parent mesh if current primitive > 0
      // and set it as target mesh
      if (p > 0) {
        target_mesh = scene_new_mesh(scene);

        // add target mesh pointer to parent mesh children list
        mesh_child_add(scene_mesh, target_mesh);

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
      // Use default pbr shader as default
      // TODO: Add a custom path for different shader in loader configuration
      cgltf_material *material = current_primitive.material;

      mesh_shader_create(target_mesh,
                         &(ShaderCreateDescriptor){
                             .pipeline = std_pipeline(PipelineType_PBR),
                             .label = material->name,
                             .name = material->name,
                             .device = device,
                             .queue = queue,
                         });

      loader_gltf_bind_uniforms(mesh_shader_texture(target_mesh), material,
                                options);

      // define mesh vertex attribute
      mesh_topology_base_create(&target_mesh->topology.base, &vert_attr,
                                &vert_index, target_mesh->device,
                                target_mesh->queue);

      // set mesh position
      loader_gltf_mesh_position(scene_mesh, gl_mesh.name, data);

      scene_add_mesh(scene, target_mesh, NULL);
    }
  }
}

/**
  Bind PBR textures
  store the texture_views (hold pointer to actual texture + other data)
 */
void loader_gltf_bind_uniforms(Shader *shader, cgltf_material *material,
                               const LoaderGLTFOptions *options) {

  const uint8_t texture_length = 5;

  // TODO: check how to handle if object already has a AO Texture imported ?
  // overwrite ?
  //&material->occlusion_texture : baked separately in the AO pass,
  cgltf_texture_view *texture_view_list[] = {
      &material->pbr_metallic_roughness.base_color_texture,
      &material->pbr_metallic_roughness.metallic_roughness_texture,
      &material->normal_texture,
      &material->emissive_texture,
      &material->occlusion_texture,
  };

  ShaderBindGroupSamplerEntry samplers[texture_length];

  uint8_t binding = 0;

  // get the cached fallback texture view from renderer
  const WGPUTextureView fallback_texture =
      std_texture_view(TextureViewType_Float);

  for (int t = 0; t < texture_length; t++) {

    void *data;
    size_t size;
    int width, height, channels;

    // If find texture, upload new texture to GPU and bind to shader
    //(before freeing it)
    if (loader_gltf_extract_texture(
            texture_view_list[t], &data, &size, &width, &height, &channels,
            options->max_texture_size) == LoaderGLTFStatus_TextureFound) {

      // send texture + sampler to shader
      shader_update_texture(shader, SHADER_TEXTURE_BINDGROUP_TEXTURES, binding,
                            &(ShaderUpdateTexture){
                                .data = data,
                                .size = size,
                                .width = width,
                                .height = height,
                                .dimension = WGPUTextureViewDimension_2D,
                                .format = WGPUTextureFormat_BGRA8Unorm,
                                .channels = TextureChannel_RGBA,
                            });
    }

    // update sampler entry from generated array
    shader_update_sampler(shader, SHADER_TEXTURE_BINDGROUP_TEXTURES,
                          binding + 1,
                          &(WGPUSamplerDescriptor){
                              .addressModeU = WGPUAddressMode_ClampToEdge,
                              .addressModeV = WGPUAddressMode_ClampToEdge,
                              .addressModeW = WGPUAddressMode_ClampToEdge,
                              .minFilter = WGPUFilterMode_Linear,
                              .magFilter = WGPUFilterMode_Linear,
                              .compare = WGPUCompareFunction_Undefined,
                          });
    binding += 2;
  }
}

/**
    Extract textures from texture_view
    1. if uri => load image
    2. if buffer_view => store buffer & size
 */
LoaderGLTFStatus loader_gltf_extract_texture(cgltf_texture_view *texture_view,
                                             void **data, size_t *size,
                                             int *width, int *height,
                                             int *channels,
                                             TextureResolution max_size) {

  const TextureChannel forced_channel = TextureChannel_RGBA;

  if (texture_view->texture) {

    // TODO: check why cgltf buffer->size return smaller size that w * h *
    // channels
    cgltf_image *image = texture_view->texture->image;
    if (image->uri) {
      cgltf_decode_uri(image->uri);
      *data = stbi_load(image->uri, width, height, channels, forced_channel);

    } else if (image->buffer_view) {

      unsigned char *gltf_data =
          (unsigned char *)image->buffer_view->buffer->data +
          image->buffer_view->offset;

      // use stbi to convert gltf image from RGB(A) to RGBA, ensuring 4 channels
      // TODO: more flexible texture upload (RGB/RGBA, large texture
      // handling...)
      *data = stbi_load_from_memory(gltf_data, image->buffer_view->buffer->size,
                                    width, height, channels, forced_channel);

    } else {
      VERBOSE_PRINT(
          "Loader GLTF: Texture found but couldn't be loaded, loading "
          "default texture");
      return LoaderGLTFStatus_LoadError;
    }

    if (*data != NULL) {

      if (*width > max_size || *height > max_size) {

        int n_w = max_size;
        int n_h = max_size;

        if (*width > *height)
          n_h = (int)((float)*height * max_size / *width);
        else
          n_w = (int)((float)*width * max_size / *height);

        n_w = glm_max(1, n_w);
        n_h = glm_max(1, n_h);

        unsigned char *n_data = malloc(n_w * n_h * forced_channel);

        if (n_data == NULL) {
          VERBOSE_WARNING(
              "GLTF Loader couldn't allocate resources for resize texture.");
        } else if (stbir_resize_uint8_srgb(*data, *width, *height, 0, n_data,
                                           n_w, n_h, 0,
                                           (uint8_t)forced_channel) == NULL) {
          VERBOSE_WARNING("GLTF Loader STBI resize texture fail.");
        } else {

          *width = n_w;
          *height = n_h;

          // free old texture
          stbi_image_free(*data);

          *data = n_data;
        }
      }

      *size = (*width) * (*height) * forced_channel;
      return LoaderGLTFStatus_TextureFound;
    } else {
      return LoaderGLTFStatus_LoadError;
    }

  } else {
    VERBOSE_PRINT(
        "Loader GLTF: Couldn't find texture, loading default texture");
    return LoaderGLTFStatus_TextureUnfound;
  }

  return LoaderGLTFStatus_UndefError;
}

void loader_gltf_mesh_position(Mesh *mesh, const char *name, cgltf_data *data) {

  // Apply transformation to mesh
  // Transformation attributes are stored in the nodes
  // whereas mesh only contain vertices/index related data
  // need to go through the nodes and compare with the given gltf_mesh to see
  // if it matches name

  for (size_t n = 0; n < data->nodes_count; n++) {

    cgltf_node *node = &data->nodes[n];
    if (strcmp(node->mesh->name, name) == 0) {

      // set translation
      if (node->has_translation)
        mesh_set_position(mesh, (vec3){
                                    node->translation[0],
                                    node->translation[1],
                                    node->translation[2],
                                });

      // set scale
      if (node->has_scale)
        mesh_set_scale(mesh, (vec3){
                                 node->scale[0],
                                 node->scale[1],
                                 node->scale[2],
                             });
      // set rotation
      if (node->has_rotation)
        mesh_set_rotation_quat(mesh, node->rotation);
    }
  }
}
