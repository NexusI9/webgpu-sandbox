#include "skybox.h"
#include "../../../backend/buffer.h"
#include "../../../include/stb/stb_image.h"
#include "../../primitive/cube.h"
#include "../../texture/texture.h"
#include "webgpu/webgpu.h"
#include <stdint.h>
#include "../runtime/mesh/shader/shader.h"

static inline WGPUTexture prefab_skybox_texture(const WGPUDevice, const size_t);

static inline void prefab_skybox_create_layer(const WGPUTexture,
                                              const Texture *, const size_t,
                                              const WGPUQueue,
                                              BufferTextureMemory);

static inline void prefab_skybox_create_from_texture(Scene *, const WGPUTexture,
                                                     WGPUTextureView *,
                                                     const size_t, const float);

static const int layer_count = 6;
static const WGPUTextureFormat format = WGPUTextureFormat_RGBA8Unorm;

/**
   Upload the skybox side to the gpu
 */
static inline void prefab_skybox_create_layer(const WGPUTexture texture,
                                              const Texture *layer_texture,
                                              const size_t layer_index,
                                              const WGPUQueue queue,
                                              BufferTextureMemory free) {

  buffer_create_texture_cube(
      &(CreateTextureCubeDescriptor){
          .texture = &texture,
          .queue = queue,
          .width = layer_texture->width,
          .height = layer_texture->height,
          .size = layer_texture->size,
          .data = layer_texture->data,
          .channels = layer_texture->channels,
          .format = format,
          .layer = layer_index,
      },
      free);
}

/**
  Create texture & global texture view
 */
WGPUTexture prefab_skybox_texture(const WGPUDevice device,
                                  const size_t resolution) {
  return wgpuDeviceCreateTexture(
      device,
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
void prefab_skybox_create_from_texture(Scene *scene, const WGPUTexture texture,
                                       WGPUTextureView *view,
                                       const size_t resolution,
                                       const float blur) {

  // create global texture view
  *view = wgpuTextureCreateView(texture,
                                &(WGPUTextureViewDescriptor){
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
  Mesh *skybox_mesh = scene_new_mesh(scene);
  mesh_create_primitive(skybox_mesh, &(MeshCreatePrimitiveDescriptor){
                                         .device = scene_device(scene),
                                         .queue = scene_queue(scene),
                                         .name = "skybox mesh",
                                         .primitive = &box_primitive,
                                     });

  // assign shader
  mesh_shader_create_fixed(skybox_mesh,
                           &(ShaderCreateDescriptor){
                               .device = scene_device(scene),
                               .queue = scene_queue(scene),
                               .label = "skybox shader",
                               .name = "skybox shader",
                               .pipeline = std_pipeline(PipelineType_Skybox),
                           });

  // update texture and sampler
  Shader *shader = mesh_shader_fixed(skybox_mesh);
  shader_update_texture_view(shader, 0, 0, *view, format);
  shader_update_sampler(shader, 0, 1,
                        &(WGPUSamplerDescriptor){
                            .addressModeU = WGPUAddressMode_ClampToEdge,
                            .addressModeV = WGPUAddressMode_ClampToEdge,
                            .addressModeW = WGPUAddressMode_ClampToEdge,
                            .minFilter = WGPUFilterMode_Linear,
                            .magFilter = WGPUFilterMode_Linear,
                            .compare = WGPUCompareFunction_Undefined,
                        });

  // add blur uniform
  shader_update_uniform(shader, 0, 2, (void *)&blur);

  // alter pipeline (no depth test)
  const Pipeline *pipeline = shader_pipeline(shader);

  scene_add_mesh_fixed(scene, skybox_mesh, ScenePipeline_Fixed_Background,
                       NULL);
}

/**
   Create a skybox from a list of 6 textures.
 */
void prefab_skybox_create(Scene *scene,
                          const PrefabSkyboxCreateDescriptor *desc) {

  // create global texture
  WGPUTexture *skybox_texture = &scene->renderer.texture.skybox.texture;
  WGPUTextureView *skybox_cubemap_view =
      &scene->renderer.texture.skybox.cubemap;

  texture_create_cubemap_from_file(skybox_texture,
                                   &(TextureCreateCubeMapDescriptor){
                                       .device = scene_device(scene),
                                       .queue = scene_queue(scene),
                                       .path = &desc->path,
                                       .format = format,
                                       .resolution = desc->resolution,
                                   });

  prefab_skybox_create_from_texture(scene, *skybox_texture, skybox_cubemap_view,
                                    desc->resolution, desc->blur);
}

/**
   Create a gradient skybox from a list of gradient stops.
 */
void prefab_skybox_gradient_create(
    Scene *scene, const PrefabSkyboxGradientCreateDescriptor *desc) {

  WGPUTexture *skybox_texture = &scene->renderer.texture.skybox.texture;
  WGPUTextureView *skybox_cubemap_view =
      &scene->renderer.texture.skybox.cubemap;

  // create global texture
  *skybox_texture =
      prefab_skybox_texture(scene_device(scene), desc->resolution);

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

  // precompute gradient sides to be reused for each cube sides
  Texture gradient_texture;
  texture_create(&gradient_texture, &(TextureCreateDescriptor){
                                        .channels = 4,
                                        .height = desc->resolution,
                                        .width = desc->resolution,
                                        .value = end->color,
                                    });

  // create gradient
  texture_write_gradient(&gradient_texture, &desc->stops,
                         TextureWriteMethod_Replace);

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
    Texture *final_texture;
    BufferTextureMemory free_texture = BufferTextureMemory_Free;

    switch (i) {

    // top
    case 2:
      texture_create(&layer_texture, &(TextureCreateDescriptor){
                                         .channels = 4,
                                         .height = desc->resolution,
                                         .width = desc->resolution,
                                         .value = start->color,
                                     });
      final_texture = &layer_texture;
      break;

    // bottom
    case 3:
      texture_create(&layer_texture, &(TextureCreateDescriptor){
                                         .channels = 4,
                                         .height = desc->resolution,
                                         .width = desc->resolution,
                                         .value = end->color,
                                     });
      final_texture = &layer_texture;
      break;

    // sides
    default:
      final_texture = &gradient_texture;
      free_texture = BufferTextureMemory_Keep;
    }

    // upload texture
    prefab_skybox_create_layer(*skybox_texture, final_texture, i,
                               scene_queue(scene), free_texture);
  }

  // free gradient texture
  stbi_image_free(gradient_texture.data);
  gradient_texture.data = NULL;

  prefab_skybox_create_from_texture(scene, *skybox_texture, skybox_cubemap_view,
                                    desc->resolution, 0.0f);
}
