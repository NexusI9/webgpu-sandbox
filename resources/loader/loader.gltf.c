#include "loader.gltf.h"

#include <cglm/types.h>
#include <cglm/util.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend/std_pipeline/core.h"
#include "backend/std_pipeline/modules/pbr/pbr.h"
#include "backend/std_texture/core.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/core.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/geometry/vertex/list.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/topology/base.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/core.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "utils/system.h"
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

// vertex buffer utils
static void loader_gltf_primitive_vertex_index(VertexIndex *,
                                               cgltf_primitive *);
static inline void loader_gltf_primitive_vertex_attribute_add(VertexAttribute *,
                                                              float *, size_t,
                                                              size_t, uint8_t);
static inline void
loader_gltf_primitive_vertex_attribute_create(VertexList *, VertexAttribute *,
                                              cgltf_primitive *);
static inline void loader_gltf_primitie_vertex_lists_init(VertexAttribute *,
                                                          VertexList *, size_t);

// mesh utils
static LoaderGLTFStatus loader_gltf_create_mesh(Scene *, const WGPUDevice,
                                                const WGPUQueue, cgltf_data *,
                                                const LoaderGLTFOptions *,
                                                LoaderGLTFResult *);
static void loader_gltf_mesh_position(Mesh *, const char *, cgltf_data *);

// shader utils
static inline void loader_gltf_bind_textures(Mesh *, cgltf_material *,
                                             const LoaderGLTFOptions *);

static inline void loader_gltf_bind_uniforms(Mesh *, cgltf_material *,
                                             const LoaderGLTFOptions *);

static LoaderGLTFStatus loader_gltf_extract_texture(cgltf_texture_view *,
                                                    void **, size_t *, int *,
                                                    int *, int *,
                                                    TextureResolution);

LoaderGLTFStatus loader_gltf_load(const GLTFLoadDescriptor *desc,
                                  LoaderGLTFResult *dest) {

  VERBOSE_IMPORT("GLTF file: %s", desc->path);

  cgltf_data *data = NULL;
  cgltf_result result;

  // load json structure
  TIMER("GLTF Parse file",
        { result = cgltf_parse_file(desc->cgltf_options, desc->path, &data); });

  // load actual gltf buffer data
  TIMER("GLTF Load Buffer", {
    result = cgltf_load_buffers(desc->cgltf_options, data, desc->path);
  });

  if (dest)
    *dest = (LoaderGLTFResult){0};

  switch (result) {

  case cgltf_result_invalid_json:
    VERBOSE_ERROR("Invalid GLTF JSON.");
    return LoaderGLTFStatus_JSONInvalid;
    break;

  case cgltf_result_success:
    return loader_gltf_create_mesh(desc->scene, desc->device, desc->queue, data,
                                   desc->options, dest);
    break;

  case cgltf_result_file_not_found:
    VERBOSE_ERROR("GLTF file not found.");
    return LoaderGLTFStatus_FileUnfound;

  case cgltf_result_out_of_memory:
    VERBOSE_ERROR("GLTF loading aborted, out of memory.");
    return LoaderGLTFStatus_OutOfBoundMemory;

  default:
    VERBOSE_ERROR("GLTF loading aborted, unhanded error.");
    return LoaderGLTFStatus_UndefError;
  }

  cgltf_free(data);

  return LoaderGLTFStatus_Success;
}

void loader_gltf_primitive_vertex_attribute_add(VertexAttribute *vert_attribute,
                                                float *data, size_t offset,
                                                size_t count,
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

void loader_gltf_primitive_vertex_lists_init(VertexAttribute *attributes,
                                             VertexList *list, size_t count) {

  // init vertex list
  vertex_list_create(list, count);

  // init vertex data (interleaved attributes)
  // list->count is the number of vertex used by index array
  // need to multiply by stride
  attributes->length = list->count * VERTEX_STRIDE;
  attributes->capacity = attributes->length;
  attributes->entries =
      (vattr_t *)calloc(attributes->capacity, sizeof(vattr_t));
}

static inline void
loader_gltf_primitive_vertex_attribute_create(VertexList *vert_list,
                                              VertexAttribute *vert_attr,
                                              cgltf_primitive *primitive) {

  static const struct {
    VertexAttributeType type;
    VertexAttributeDimension dimension;
    VertexAttributeOffset offset;
  } type_vertex_map[cgltf_attribute_type_max_enum] = {
      [cgltf_attribute_type_position] =
          {
              .type = VertexAttributeType_Position,
              .dimension = VertexAttributeDimension_Position,
              .offset = VertexAttributeOffset_Position,
          },
      [cgltf_attribute_type_normal] =
          {
              .type = VertexAttributeType_Normal,
              .dimension = VertexAttributeDimension_Normal,
              .offset = VertexAttributeOffset_Normal,
          },
      [cgltf_attribute_type_tangent] =
          {
              .type = VertexAttributeType_Tangent,
              .dimension = VertexAttributeDimension_Tangent,
              .offset = VertexAttributeOffset_Tangent,
          },
      [cgltf_attribute_type_color] =
          {
              .type = VertexAttributeType_Color,
              .dimension = VertexAttributeDimension_Color,
              .offset = VertexAttributeOffset_Color,
          },
      [cgltf_attribute_type_texcoord] =
          {
              .type = VertexAttributeType_Uv,
              .dimension = VertexAttributeDimension_Uv,
              .offset = VertexAttributeOffset_Uv,
          },
  };

  for (size_t a = 0; a < primitive->attributes_count; a++) {

    cgltf_attribute *attribute = &primitive->attributes[a];
    cgltf_accessor *accessor = attribute->data;
    cgltf_attribute_type type = attribute->type;

    if (type_vertex_map[type].dimension == 0)
      continue;

    // first concat each attribute in their respective list (all pos
    // together etc.)
    loader_gltf_accessor_to_array(
        accessor, vert_list->attributes[type_vertex_map[type].type],
        type_vertex_map[type].dimension);

    // interleave vertex data ( create pattern pos / norm / tan / uv...)
    loader_gltf_primitive_vertex_attribute_add(
        vert_attr, vert_list->attributes[type_vertex_map[type].type],
        type_vertex_map[type].offset, vert_list->count,
        type_vertex_map[type].dimension);
  }
}

static void loader_gltf_accessor_to_array(cgltf_accessor *accessor,
                                          float *destination,
                                          uint8_t dimension) {

  float *attributes = loader_gltf_attributes(accessor);
  size_t index = 0;
  for (size_t a = 0; a < accessor->count; a++)
    for (uint8_t u = 0; u < dimension; u++)
      destination[index++] = attributes[a * dimension + u];
}

void loader_gltf_primitive_vertex_index(VertexIndex *vert_index,
                                        cgltf_primitive *source) {

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

  *vert_index = (VertexIndex){
      .entries = index_data,
      .capacity = index_count,
      .length = index_count,
  };
}

LoaderGLTFStatus loader_gltf_create_mesh(Scene *scene, const WGPUDevice device,
                                         const WGPUQueue queue,
                                         cgltf_data *data,
                                         const LoaderGLTFOptions *options,
                                         LoaderGLTFResult *result) {

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
      {
        loader_gltf_primitive_vertex_lists_init(
            &vert_attr, &vert_list,
            current_primitive.attributes[0].data->count);

        loader_gltf_primitive_vertex_attribute_create(&vert_list, &vert_attr,
                                                      &current_primitive);
        loader_gltf_primitive_vertex_index(&vert_index, &current_primitive);
      }

      // target current mesh itself if primitive == 0
      Mesh *target_mesh = scene_mesh;

      // add child to parent mesh if current primitive > 0
      // and set it as target mesh
      if (p > 0) {
        target_mesh = scene_new_mesh(scene);
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

      // === CREATE SHADER AND BIND TEXTURE/UNIFORMS
      {
        // Use default pbr shader as default
        // TODO: Add a custom path for different shader in loader configuration
        cgltf_material *material = current_primitive.material;

        printf("mesh: %s <=> material: %s\n", target_mesh->name,
               material->name);
        mesh_shader_create(target_mesh,
                           &(ShaderCreateDescriptor){
                               .pipeline = std_pipeline(PipelineType_PBR),
                               .label = material->name,
                               .name = material->name,
                               .device = device,
                               .queue = queue,
                           });

        // load and bind gltf textures
        loader_gltf_bind_textures(target_mesh, material, options);
        loader_gltf_bind_uniforms(target_mesh, material, options);
      }

      // define mesh vertex attribute
      mesh_topology_base_create(&target_mesh->topology.base, &vert_attr,
                                &vert_index, target_mesh->device,
                                target_mesh->queue);

      // set mesh position
      loader_gltf_mesh_position(scene_mesh, gl_mesh.name, data);

      scene_add_mesh(scene, target_mesh, NULL);

      // ==== UPDATE STATS ===
      {
        if (result) {
          result->stats.mesh_count++;
          result->stats.vertex_count +=
              target_mesh->topology.base.attribute.length / VERTEX_STRIDE;

          if (result->meshes.length < LOADER_GLTF_RESULT_MESH_COUNT)
            result->meshes.entries[result->meshes.length++] = target_mesh;
        }
      }
    }
  }

  return LoaderGLTFStatus_Success;
}

/**
  Bind PBR textures
  store the texture_views (hold pointer to actual texture + other data)
 */
void loader_gltf_bind_textures(Mesh *mesh, cgltf_material *material,
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
      ShaderBindGroupTextureEntry *shader_texture =
          shader_update_texture(mesh_shader(mesh, MeshShader_Texture),
                                SHADER_TEXTURE_BINDGROUP_TEXTURES, binding,
                                &(ShaderUpdateTexture){
                                    .data = data,
                                    .size = size,
                                    .width = width,
                                    .height = height,
                                    .dimension = WGPUTextureViewDimension_2D,
                                    .format = TEXTURE_FORMAT_OFFSCREEN_DEFAULT,
                                    .channels = TextureChannel_RGBA,
                                });

      // transfert texture view to reflection shader (reuse resource), however
      // need to be careful with shared ownership. Here it shouldn't be to
      // much trouble since reflection and texture shader lifetime are mostly
      // linked.
      shader_update_texture_view(mesh_shader(mesh, MeshShader_Reflection),
                                 SHADER_TEXTURE_BINDGROUP_TEXTURES, binding,
                                 shader_texture->texture_view,
                                 shader_texture->format);
    }

    binding += 2;
  }
}

/**
   Read material factors and load it in the PBRUniform
 */
void loader_gltf_bind_uniforms(Mesh *mesh, cgltf_material *material,
                               const LoaderGLTFOptions *options) {

  Shader *texture_shader = mesh_shader(mesh, MeshShader_Texture);
  Shader *reflection_shader = mesh_shader(mesh, MeshShader_Reflection);

  PBRMaterialUniform pbr = {0};

  pbr.metallic_factor = material->pbr_metallic_roughness.metallic_factor;
  pbr.roughness_factor = material->pbr_metallic_roughness.roughness_factor;
  pbr.specular_factor = material->specular.specular_factor;
  pbr.normal_scale = material->normal_texture.scale;
  pbr.occlusion_strength = material->occlusion_texture.scale;
  glm_vec3_copy(material->emissive_factor, pbr.emissive_factor);
  glm_vec4_copy(material->pbr_metallic_roughness.base_color_factor,
                pbr.base_color_factor);

  shader_update_uniform_data(texture_shader, 1, 10, &pbr);
  shader_update_uniform_data(reflection_shader, 1, 10, &pbr);

  {
    // DELETE ME DEBUG
    printf("[[%s]]\n", material->name);
    printf("\tmetallic: %f\n", pbr.metallic_factor);
    printf("\troughness: %f\n", pbr.roughness_factor);
    printf("\tocclusion: %f\n", pbr.occlusion_strength);
    printf("\tspecular: %f\n", pbr.roughness_factor);
    printf("\tnormal: %f\n", pbr.normal_scale);
    printf("\temissive:");
    print_vec3(pbr.emissive_factor);
    printf("\tbase color:");
    print_vec4(pbr.base_color_factor);
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
      TIMER("GLTF Load Texture", {
        *data = stbi_load(image->uri, width, height, channels, forced_channel);
      });

    } else if (image->buffer_view) {

      unsigned char *gltf_data =
          (unsigned char *)image->buffer_view->buffer->data +
          image->buffer_view->offset;

      // use stbi to convert gltf image from RGB(A) to RGBA, ensuring 4
      // channels
      // TODO: more flexible texture upload (RGB/RGBA, large texture
      // handling...)
      TIMER("GLTF Load Texture", {
        *data =
            stbi_load_from_memory(gltf_data, image->buffer_view->buffer->size,
                                  width, height, channels, forced_channel);
      });

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
