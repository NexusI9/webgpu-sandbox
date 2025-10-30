#include "loader.gltf.h"

#include "utils/system.h"
#include <cglm/types.h>
#include <cglm/util.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend/logger.h"
#include "backend/std_pipeline/core.h"
#include "backend/std_pipeline/render_shader/pbr/pbr.h"
#include "backend/std_texture/core.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/core.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/geometry/vertex/list.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/topology/base.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/shader/bindgroup.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "utils/name.h"
#include "webgpu/webgpu.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"

// gltf utils
static float *loader_gltf_attributes(const cgltf_accessor *);

static inline void
loader_gltf_set_attribute_fallback(float *, const float *,
                                   const VertexAttributeDimension);
static inline bool
loader_gltf_attribute_is_empty(const float *, const VertexAttributeDimension);

// vertex buffer utils
static void loader_gltf_primitive_vertex_index(VertexIndex *,
                                               cgltf_primitive *);

static inline void
loader_gltf_primitive_vertex_attribute_create(VertexAttribute *,
                                              cgltf_primitive *);
static inline void loader_gltf_primitie_vertex_lists_init(VertexAttribute *,
                                                          size_t);

// mesh utils
static inline LoaderGLTFStatus
loader_gltf_traverse_nodes(cgltf_data *, Scene *, const LoaderGLTFOptions *,
                           LoaderGLTFResult *);
static inline LoaderGLTFStatus
loader_gltf_create_mesh(Scene *, cgltf_node *, Mesh *,
                        const LoaderGLTFOptions *, LoaderGLTFResult *);
static inline void loader_gltf_mesh_position(cgltf_node *, Mesh *);

// shader utils
static inline void loader_gltf_bind_textures(Mesh *, cgltf_material *,
                                             const LoaderGLTFOptions *);

static inline void loader_gltf_bind_uniforms(Mesh *, cgltf_material *,
                                             const LoaderGLTFOptions *);

static LoaderGLTFStatus
loader_gltf_extract_texture(cgltf_texture_view *, void **, size_t *, int *,
                            int *, int *, TextureResolution, const char *);

LoaderGLTFStatus loader_gltf_load(const GLTFLoadDescriptor *desc,
                                  LoaderGLTFResult *dest) {

  logger_add(LoggerFlag_Import, "GLTF file: %s", desc->path);

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
    logger_add(LoggerFlag_Error, "Invalid GLTF JSON.");
    return LoaderGLTFStatus_JSONInvalid;
    break;

  case cgltf_result_success:
    return loader_gltf_traverse_nodes(data, desc->scene, desc->options, dest);
    break;

  case cgltf_result_file_not_found:
    logger_add(LoggerFlag_Error, "GLTF file not found.");
    return LoaderGLTFStatus_FileUnfound;

  case cgltf_result_out_of_memory:
    logger_add(LoggerFlag_Error, "GLTF loading aborted, out of memory.");
    return LoaderGLTFStatus_OutOfBoundMemory;

  default:
    logger_add(LoggerFlag_Error, "GLTF loading aborted, unhanded error.");
    return LoaderGLTFStatus_UndefError;
  }

  cgltf_free(data);

  return LoaderGLTFStatus_Success;
}

void loader_gltf_set_attribute_fallback(float *attr, const float *fallback,
                                        const VertexAttributeDimension count) {
  for (VertexAttributeDimension i = 0; i < count; i++)
    attr[i] = fallback[i];
}

bool loader_gltf_attribute_is_empty(const float *attr,
                                    const VertexAttributeDimension count) {

  for (VertexAttributeDimension i = 0; i < count; i++)
    if (attr[i])
      return false;

  return true;
}

LoaderGLTFStatus loader_gltf_traverse_nodes(cgltf_data *data, Scene *scene,
                                            const LoaderGLTFOptions *options,
                                            LoaderGLTFResult *result) {

  for (size_t i = 0; i < data->nodes_count; i++) {
    cgltf_node *node = &data->nodes[i];
    if (node->mesh)
      loader_gltf_create_mesh(scene, node, NULL, options, result);
  }


  return LoaderGLTFStatus_Success;
}

float *loader_gltf_attributes(const cgltf_accessor *accessor) {

  cgltf_buffer_view *buffer_view = accessor->buffer_view;

  // combine accessor and buffer view offset
  size_t offset = buffer_view->offset + accessor->offset;
  return (float *)((uint8_t *)buffer_view->buffer->data + offset);
}

void loader_gltf_primitive_vertex_lists_init(VertexAttribute *attributes,
                                             size_t count) {
  attributes->length = count;
  attributes->capacity = attributes->length;
  attributes->entries =
      (vattr_t *)calloc(attributes->capacity, sizeof(vattr_t));
}

/**
   Access vertex data from the file and interleave them within the referenced
   vertex attribute. Note that initially in GLTF format, all the vertex
   attributes are packed by attributes, meaning we have: [p1, p2, p3, p4, p5]
   [n1, n1, n3, n4, n5]
   etc.
 */
static inline void
loader_gltf_primitive_vertex_attribute_create(VertexAttribute *vert_attr,
                                              cgltf_primitive *primitive) {

  const struct {
    const cgltf_attribute_type gltf_attribute;
    const VertexAttributeType type;
    const VertexAttributeDimension dimension;
    const VertexAttributeOffset offset;
    const vattr_t (*fallback)[4];
  } type_vertex_map[VERTEX_ATTRIBUTE_COUNT] = {
      {
          .gltf_attribute = cgltf_attribute_type_position,
          .type = VertexAttributeType_Position,
          .dimension = VertexAttributeDimension_Position,
          .offset = VertexAttributeOffset_Position,
      },
      {
          .gltf_attribute = cgltf_attribute_type_normal,
          .type = VertexAttributeType_Normal,
          .dimension = VertexAttributeDimension_Normal,
          .offset = VertexAttributeOffset_Normal,
          .fallback = &(vattr_t[4]){0.0f, 1.0f, 0.0f},
      },
      {
          .gltf_attribute = cgltf_attribute_type_tangent,
          .type = VertexAttributeType_Tangent,
          .dimension = VertexAttributeDimension_Tangent,
          .offset = VertexAttributeOffset_Tangent,
          .fallback = &(vattr_t[4]){-1.0f, 0.0f, 0.0f, -1.0f},
      },
      {
          .gltf_attribute = cgltf_attribute_type_color,
          .type = VertexAttributeType_Color,
          .dimension = VertexAttributeDimension_Color,
          .offset = VertexAttributeOffset_Color,
      },
      {
          .gltf_attribute = cgltf_attribute_type_texcoord,
          .type = VertexAttributeType_Uv,
          .dimension = VertexAttributeDimension_Uv,
          .offset = VertexAttributeOffset_Uv,
      },
  };

  // traverse and check if gltf attribute match
  for (size_t j = 0; j < primitive->attributes_count; j++) {

    const cgltf_attribute *attribute = &primitive->attributes[j];
    const cgltf_accessor *accessor = attribute->data;
    const cgltf_attribute_type gltf_type = attribute->type;

    float *data = loader_gltf_attributes(accessor);

    for (size_t i = 0; i < VERTEX_ATTRIBUTE_COUNT; i++) {

      size_t index = 0;
      const VertexAttributeDimension dimension = type_vertex_map[i].dimension;
      const VertexAttributeOffset offset = type_vertex_map[i].offset;
      const cgltf_attribute_type gltf_attr = type_vertex_map[i].gltf_attribute;

      if (primitive->attributes[j].type == gltf_attr) {

        for (size_t k = 0; k < accessor->count * dimension; k += dimension) {
          memcpy(&vert_attr->entries[index + offset], &data[k],
                 sizeof(vattr_t) * dimension);

          index += VERTEX_STRIDE;
        }
      }
    }
  }
}

static void
loader_gltf_accessor_to_array(const cgltf_accessor *accessor,
                              float *destination,
                              const VertexAttributeDimension dimension) {

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

LoaderGLTFStatus loader_gltf_create_mesh(Scene *scene, cgltf_node *gl_node,
                                         Mesh *parent,
                                         const LoaderGLTFOptions *options,
                                         LoaderGLTFResult *result) {

  cgltf_mesh *gl_mesh = gl_node->mesh;
  struct Mesh *scene_mesh = scene_new_mesh(scene);
  mesh_create(scene_mesh, &(MeshCreateDescriptor){
                              .name = gl_mesh->name,
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

    TODO :
    For now we include primitives as child along with the other child meshes.
    But maybe in the future it could be useful to put them under a primitive
    array.
  */
  for (size_t p = 0; p < gl_mesh->primitives_count; p++) {
    // get accessors to decode buffers into typed data (vertex, indices...)
    // load vertex attributes

    VertexAttribute vert_attr = {0};
    VertexIndex vert_index = {0};

    cgltf_primitive current_primitive = gl_mesh->primitives[p];

    // Initialize vertex lists with 0.0:
    // need fallback values in case no color or uv coordinates
    // ensure to maintain correct standaridzed structure for shaders
    {

      const size_t vertex_count =
          current_primitive.attributes[0].data->count * VERTEX_STRIDE;

      loader_gltf_primitive_vertex_lists_init(&vert_attr, vertex_count);

      loader_gltf_primitive_vertex_attribute_create(&vert_attr,
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

      char mesh_name[NAME_LEN];
      name_compose(mesh_name, "%s.%lu", gl_mesh->name, p);

      mesh_create(target_mesh, &(MeshCreateDescriptor){
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

      RenderPipelineType pipeline_type = RenderPipelineType_PBR;

      if (material->double_sided)
        pipeline_type = RenderPipelineType_PBR_DoubleSided;

      if (material->alpha_mode == cgltf_alpha_mode_blend)
        pipeline_type = RenderPipelineType_PBR_Alpha;

      mesh_shader_create(target_mesh,
                         &(ShaderCreateDescriptor){
                             .pipeline = std_render_pipeline(pipeline_type),
                             .name = material->name,
                         });

      // load and bind gltf textures
      loader_gltf_bind_textures(target_mesh, material, options);
      loader_gltf_bind_uniforms(target_mesh, material, options);
    }

    loader_gltf_mesh_position(gl_node, target_mesh);
    // define mesh vertex attribute
    mesh_topology_base_create(&target_mesh->topology.base, &vert_attr,
                              &vert_index);

    scene_add_mesh(scene, target_mesh, NULL, SceneAddFlag_None);

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

    for (size_t i = 0; i < gl_node->children_count; i++)
      loader_gltf_create_mesh(scene, gl_node->children[i], target_mesh, options,
                              result);
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
  const struct {
    const char *label;
    cgltf_texture_view *gltf_texture;
    const WGPUTextureView fallback_texture;
  } texture_view_list[] = {
      {
          "Base",
          &material->pbr_metallic_roughness.base_color_texture,
          std_texture_view(TextureViewType_Float),
      },
      {
          "Metallic Roughness",
          &material->pbr_metallic_roughness.metallic_roughness_texture,
          std_texture_view(TextureViewType_Float),
      },
      {
          "Normal",
          &material->normal_texture,
          std_texture_view(TextureViewType_FloatNormal),
      },
      {
          "Emissive",
          &material->emissive_texture,
          std_texture_view(TextureViewType_Float),
      },
      {
          "Occlusion",
          &material->occlusion_texture,
          std_texture_view(TextureViewType_Float),
      },
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
    ShaderBindGroupTextureEntry *reflection_texture;
    if (loader_gltf_extract_texture(
            texture_view_list[t].gltf_texture, &data, &size, &width, &height,
            &channels, options->max_texture_size,
            texture_view_list[t].label) == LoaderGLTFStatus_TextureFound) {

      // send texture + sampler to shader
      reflection_texture =
          shader_update_texture(mesh_shader(mesh, MeshShader_Texture),
                                SHADER_TEXTURE_BINDGROUP_TEXTURES, binding,
                                &(ShaderUpdateTexture){
                                    .data = data,
                                    .size = size,
                                    .width = width,
                                    .height = height,
                                    .dimension = WGPUTextureViewDimension_2D,
                                    .format = TEXTURE_FORMAT_OFFSCREEN,
                                    .channels = TextureChannel_RGBA,
                                },
                                ShaderUpdateFlag_None);
    } else {

      reflection_texture = shader_update_texture_view(
          mesh_shader(mesh, MeshShader_Texture),
          SHADER_TEXTURE_BINDGROUP_TEXTURES, binding,
          texture_view_list[t].fallback_texture, TEXTURE_FORMAT_OFFSCREEN,
          ShaderUpdateFlag_None);
    }

    // transfert texture view to reflection shader (reuse resource), however
    // need to be careful with shared ownership. Here it shouldn't be to
    // much trouble since reflection and texture shader lifetime are mostly
    // linked.
    Shader *reflection_shader = mesh_shader(mesh, MeshShader_Reflection);
    shader_update_texture_view(
        reflection_shader, SHADER_TEXTURE_BINDGROUP_TEXTURES, binding,
        reflection_texture->texture_view, reflection_texture->format,
        ShaderUpdateFlag_ReleasePrevious);

    binding++;
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
  pbr.normal_scale = material->normal_texture.scale || 1.0f;
  pbr.occlusion_strength = material->occlusion_texture.scale;
  pbr.alpha_threshold = 0.9f;
  glm_vec3_copy(material->emissive_factor, pbr.emissive_factor);
  glm_vec4_copy(material->pbr_metallic_roughness.base_color_factor,
                pbr.base_color_factor);

  static const bind_index pbr_uniform_binding = 6;
  shader_update_uniform_data(texture_shader, 1, pbr_uniform_binding, &pbr,
                             ShaderUpdateFlag_None);
  shader_update_uniform_data(reflection_shader, 1, pbr_uniform_binding, &pbr,
                             ShaderUpdateFlag_None);

  {
    // DELETE ME DEBUG
    // printf("[[%s]]\n", material->name);
    // printf("\tmetallic: %f\n", pbr.metallic_factor);
    // printf("\troughness: %f\n", pbr.roughness_factor);
    // printf("\tocclusion: %f\n", pbr.occlusion_strength);
    // printf("\tspecular: %f\n", pbr.roughness_factor);
    // printf("\tnormal: %f\n", pbr.normal_scale);
    // printf("\temissive:");
    // print_vec3(pbr.emissive_factor);
    // printf("\tbase color:");
    // print_vec4(pbr.base_color_factor);
  }
}

/**
    Extract textures from texture_view
    1. if uri => load image
    2. if buffer_view => store buffer & size
 */
LoaderGLTFStatus loader_gltf_extract_texture(
    cgltf_texture_view *texture_view, void **data, size_t *size, int *width,
    int *height, int *channels, TextureResolution max_size, const char *label) {

  const TextureChannel forced_channel = TextureChannel_RGBA;

  if (texture_view->texture) {

    // TODO: check why cgltf buffer->size return smaller size that w * h *
    // channels
    cgltf_image *image = texture_view->texture->image;
    if (image->uri) {
      cgltf_decode_uri(image->uri);

      logger_add(LoggerFlag_Import, "GLTF Texture '%s'", image->name);
      TIMER("", {
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
      logger_add(LoggerFlag_Import, "GLTF Texture '%s'", image->name);
      TIMER("", {
        *data =
            stbi_load_from_memory(gltf_data, image->buffer_view->buffer->size,
                                  width, height, channels, forced_channel);
      });

    } else {
      // logger_add(LoggerFlag_Print,
      //     "Loader GLTF: Texture found but couldn't be loaded, loading "
      //     "default texture");
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
          logger_add(
              LoggerFlag_Warning,
              "GLTF Loader couldn't allocate resources for resize texture.");
        } else if (stbir_resize_uint8_srgb(*data, *width, *height, 0, n_data,
                                           n_w, n_h, 0,
                                           (uint8_t)forced_channel) == NULL) {
          logger_add(LoggerFlag_Warning,
                     "GLTF Loader STBI resize texture fail.");
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
    logger_add(
        LoggerFlag_Print,
        "Loader GLTF: Couldn't find texture '%s', loading default texture",
        label);
    return LoaderGLTFStatus_TextureUnfound;
  }

  return LoaderGLTFStatus_UndefError;
}

void loader_gltf_mesh_position(cgltf_node *node, Mesh *mesh) {

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
