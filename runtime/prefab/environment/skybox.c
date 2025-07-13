#include "skybox.h"
#include "../../../backend/buffer.h"
#include "../../../include/stb/stb_image.h"
#include "../../primitive/cube.h"
#include "../../texture/texture.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

static inline WGPUTexture prefab_skybox_texture(WGPUDevice *, const size_t);

static inline void prefab_skybox_create_layer(const WGPUTexture *,
                                              const Texture *, const size_t,
                                              WGPUQueue *);

static inline void
prefab_skybox_create_from_texture(const PrefabCreateDescriptor *,
                                  const WGPUTexture *, const size_t,
                                  const float);

static const int layer_count = 6;
static const WGPUTextureFormat format = WGPUTextureFormat_RGBA8Unorm;

/**
   Upload the skybox side to the gpu
 */
static inline void prefab_skybox_create_layer(const WGPUTexture *texture,
                                              const Texture *layer_texture,
                                              const size_t layer_index,
                                              WGPUQueue *queue) {
  WGPUTextureView layer_texture_view = wgpuTextureCreateView(
      *texture, &(WGPUTextureViewDescriptor){
                    .format = format,
                    .dimension = WGPUTextureViewDimension_2D,
                    .arrayLayerCount = 1,
                    .baseArrayLayer = layer_index,
                    .mipLevelCount = 1,
                    .baseMipLevel = 0,
                });

  buffer_create_texture_cube(&layer_texture_view,
                             &(CreateTextureCubeDescriptor){
                                 .texture = texture,
                                 .queue = queue,
                                 .width = layer_texture->width,
                                 .height = layer_texture->height,
                                 .size = layer_texture->size,
                                 .data = layer_texture->data,
                                 .channels = layer_texture->channels,
                                 .format = format,
                                 .layer = layer_index,
                             });
}

/**
  Create texture & global texture view
 */
WGPUTexture prefab_skybox_texture(WGPUDevice *device, const size_t resolution) {
  return wgpuDeviceCreateTexture(
      *device,
      &(WGPUTextureDescriptor){
          .dimension = WGPUTextureDimension_2D,
          .format = format,
          .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
          .sampleCount = 1,
          .mipLevelCount = 1,
          .size =
              (WGPUExtent3D){
                  .width = resolution,
                  .height = resolution,
                  .depthOrArrayLayers = layer_count,
              },
      });
}

/**
   Create global view from previously generated cubemap texture and create mesh
 */
void prefab_skybox_create_from_texture(const PrefabCreateDescriptor *prefab,
                                       const WGPUTexture *texture,
                                       const size_t resolution,
                                       const float blur) {

  // create global texture view
  WGPUTextureView skybox_texture_view = wgpuTextureCreateView(
      *texture, &(WGPUTextureViewDescriptor){
                    .dimension = WGPUTextureViewDimension_Cube,
                    .format = format,
                    .arrayLayerCount = layer_count,
                    .baseArrayLayer = 0,
                    .mipLevelCount = 1,
                    .baseMipLevel = 0,
                });

  // get mesh from scene mesh pool
  /* TODO OPTI: Currently use default box primitive which include
   * normal/uv/color, but we actually only need position for the skybox, so
   * maybe can use a "position-only" version to save a bit of memory */
  Primitive box_primitive = primitive_cube();
  Mesh *skybox_mesh = scene_new_mesh_background(prefab->scene);

  mesh_create_primitive(skybox_mesh, &(MeshCreatePrimitiveDescriptor){
                                         .device = prefab->device,
                                         .queue = prefab->queue,
                                         .name = "skybox mesh",
                                         .primitive = box_primitive,
                                     });

  // assign shader
  mesh_set_shader(skybox_mesh,
                  &(ShaderCreateDescriptor){
                      .device = prefab->device,
                      .queue = prefab->queue,
                      .label = "skybox shader",
                      .name = "skybox shader",
                      .path = "./runtime/assets/shader/shader.skybox.wgsl",
                  });

  // bind texture and sampler
  Shader *shader = mesh_shader_texture(skybox_mesh);
  shader_add_texture_view(
      shader, &(ShaderCreateTextureViewDescriptor){
                  .entry_count = 1,
                  .group_index = 0,
                  .visibility = WGPUShaderStage_Fragment,
                  .entries =
                      (ShaderBindGroupTextureViewEntry[]){
                          {
                              .binding = 0,
                              .texture_view = skybox_texture_view,
                              .dimension = WGPUTextureViewDimension_Cube,
                              .format = format,
                              .sample_type = WGPUTextureSampleType_Float,
                          },
                      },
              });

  shader_add_sampler(
      shader, &(ShaderCreateSamplerDescriptor){
                  .entry_count = 1,
                  .group_index = 0,
                  .visibility = WGPUShaderStage_Fragment,
                  .entries =
                      (ShaderBindGroupSamplerEntry[]){
                          {
                              .binding = 1,
                              .addressModeU = WGPUAddressMode_ClampToEdge,
                              .addressModeV = WGPUAddressMode_ClampToEdge,
                              .addressModeW = WGPUAddressMode_ClampToEdge,
                              .minFilter = WGPUFilterMode_Linear,
                              .magFilter = WGPUFilterMode_Linear,
                              .type = WGPUSamplerBindingType_Filtering,
                              .compare = WGPUCompareFunction_Undefined,
                          },
                      },
              });

  // add blur uniform
  shader_add_uniform(shader, &(ShaderCreateUniformDescriptor){
                                 .entry_count = 1,
                                 .group_index = 0,
                                 .visibility = WGPUShaderStage_Fragment,
                                 .entries =
                                     (ShaderBindGroupUniformEntry[]){
                                         {
                                             .binding = 2,
                                             .size = sizeof(float),
                                             .offset = 0,
                                             .data = (void *)&blur,
                                             .update = NULL,
                                         },
                                     },
                             });

  // alter pipeline (no depth test)
  Pipeline *pipeline = shader_pipeline(shader);

  // set cull to front face (inside cube)
  pipeline_set_primitive(pipeline,
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_Front,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  // remove depth write, set depth comparison
  pipeline_set_stencil(pipeline,
                       (WGPUDepthStencilState){
                           .depthWriteEnabled = false,
                           .depthCompare = WGPUCompareFunction_LessEqual,
                           .format = WGPUTextureFormat_Depth24Plus,
                       });
}

/**
   Create a skybox from a list of 6 textures.
 */
void prefab_skybox_create(const PrefabCreateDescriptor *prefab,
                          const PrefabSkyboxCreateDescriptor *desc) {

  // create global texture
  const WGPUTexture skybox_texture =
      prefab_skybox_texture(prefab->device, desc->resolution);

  // put path in order
  const char *path_sort[6] = {
      desc->path.right,  // +X
      desc->path.left,   // -X
      desc->path.top,    // +Y
      desc->path.bottom, // -Y
      desc->path.front,  // +Z
      desc->path.back,   // -Z
  };

  // load image to layer textures
  for (size_t i = 0; i < layer_count; i++) {
    const char *path = path_sort[i];

    Texture layer_texture;
    if (texture_create_from_file(&layer_texture, path, false) ==
        TEXTURE_SUCCESS) {

      // upload image to gpu and update relative layer texture view
      prefab_skybox_create_layer(&skybox_texture, &layer_texture, i,
                                 prefab->queue);

      // TODO: free texture
    } else {
      perror("Couldn't read skybox texture.\n");
      return;
    }
  }

  prefab_skybox_create_from_texture(prefab, &skybox_texture, desc->resolution,
                                    desc->blur);
}

/**
   Create a gradient skybox from a list of gradient stops.
 */
void prefab_skybox_gradient_create(
    const PrefabCreateDescriptor *prefab,
    const PrefabSkyboxGradientCreateDescriptor *desc) {

  // create global texture
  const WGPUTexture skybox_texture =
      prefab_skybox_texture(prefab->device, desc->resolution);

  // define stops start and end (i.e. top and bottom color)
  const TextureGradient *grad = &desc->stops;
  TextureGradientStop *start = &grad->entries[0];
  TextureGradientStop *end = &grad->entries[desc->stops.length - 1];

  // define end/ start based on stops position
  for (size_t i = 0; i < grad->length; i++) {
    TextureGradientStop *stop = &grad->entries[0];
    if (start->position < stop->position)
      start = stop;

    if (end->position > stop->position)
      end = stop;
  }

  /* create layers, order:
     0 - right
     1 - left
     2 - top
     3 - bottom
     4 - front
     5 - back
   */
  for (size_t i = 0; i < layer_count; i++) {

    Texture layer_texture;

    switch (i) {

    // top
    case 2:
      texture_create(&layer_texture, &(TextureCreateDescriptor){
                                         .channels = 4,
                                         .height = desc->resolution,
                                         .width = desc->resolution,
                                         .value = start->color,
                                     });
      break;

    // bottom
    case 3:
      texture_create(&layer_texture, &(TextureCreateDescriptor){
                                         .channels = 4,
                                         .height = desc->resolution,
                                         .width = desc->resolution,
                                         .value = end->color,
                                     });
      break;

    // sides
    default:
      // TODO OPTI: cannot precomput gradient (resuing same pointer seems lead
      // to error), find a way to precompute it so just need to reuse the
      // gradient data throughout the side layers.
      texture_create(&layer_texture, &(TextureCreateDescriptor){
                                         .channels = 4,
                                         .height = desc->resolution,
                                         .width = desc->resolution,
                                         .value = end->color,
                                     });

      // create gradient
      texture_write_gradient(&layer_texture, &desc->stops,
                             TextureWriteMethod_Replace);
    }

    // upload texture
    prefab_skybox_create_layer(&skybox_texture, &layer_texture, i,
                               prefab->queue);
  }

  prefab_skybox_create_from_texture(prefab, &skybox_texture, desc->resolution,
                                    0.0f);
}
