#include "skybox.h"

#include "backend/buffer.h"
#include "backend/compute/mipmap.h"
#include "backend/context.h"
#include "backend/logger.h"
#include "backend/std_pipeline/core.h"
#include "include/stb/stb_image.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/primitive/cube.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/environment/core.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

static inline WGPUTexture prefab_skybox_texture(const size_t);

static inline void prefab_skybox_upload_layer(const WGPUTexture,
                                              const Texture *, const size_t,
                                              BufferTextureMemory);

static inline void prefab_skybox_create_from_texture(Scene *, const WGPUTexture,
                                                     WGPUTextureView *,
                                                     const size_t, const mip_t);

/**
   Upload the skybox side to the gpu (write to texture at given layer index)
 */
static inline void prefab_skybox_upload_layer(const WGPUTexture texture,
                                              const Texture *layer_texture,
                                              const size_t layer_index,
                                              BufferTextureMemory free) {
  buffer_create_texture_cube(
      &(CreateTextureCubeDescriptor){
          .texture = texture,
          .width = layer_texture->width,
          .height = layer_texture->height,
          .size = layer_texture->size,
          .data = layer_texture->data,
          .channels = layer_texture->channels,
          .format = TEXTURE_FORMAT_OFFSCREEN_DEFAULT,
          .layer = layer_index,
      },
      free);
}

/**
  Create texture & global texture view
 */
WGPUTexture prefab_skybox_texture(const size_t resolution) {

  return wgpuDeviceCreateTexture(
      context_device(),
      &(WGPUTextureDescriptor){
          .dimension = WGPUTextureDimension_2D,
          .format = TEXTURE_FORMAT_OFFSCREEN_DEFAULT,
          .usage = WGPUTextureUsage_TextureBinding   // read texturen in shader
                   | WGPUTextureUsage_StorageBinding // write texture in shader
                   | WGPUTextureUsage_CopyDst,       // upload the input data
          .sampleCount = 1,
          .mipLevelCount = mipmap_count(resolution, resolution),
          .size =
              (WGPUExtent3D){
                  .width = resolution,
                  .height = resolution,
                  .depthOrArrayLayers = TEXTURE_CUBE_LAYER,
              },
      });
}

/**
   Create global view from previously generated cubemap texture and create mesh
 */
void prefab_skybox_create_from_texture(Scene *scene, const WGPUTexture texture,
                                       WGPUTextureView *view,
                                       const size_t resolution,
                                       const mip_t blur) {

  // mipmap generated texture
  compute_pass_mipmap(&scene->renderer.draw.compute_pass,
                      &(MipmapDescriptor){
                          .texture = texture,
                          .layer_count = TEXTURE_CUBE_LAYER,
                      });

  // create global texture view
  const mip_t mip_count = mipmap_count(resolution, resolution);

  if (blur > mip_count)
    logger_add(LoggerFlag_Warning,
               "Attempting to set a skybox blur factor (%u) superior to "
               "the available Mip count (%u)",
               blur, mip_count);

  /* TODO:

  Looks like the textureSampleLevel is broken on Intel Mac:
   - https://issues.chromium.org/issues/372283570
   - https://github.com/gpuweb/gpuweb/issues/4818

  So we basically only make 1 mipmap view available to the shader.
  Maybe check in the future if this solution has been resolved.

   */

  *view = wgpuTextureCreateView(texture,
                                &(WGPUTextureViewDescriptor){
                                    .dimension = WGPUTextureViewDimension_Cube,
                                    .format = TEXTURE_FORMAT_OFFSCREEN_DEFAULT,
                                    .arrayLayerCount = TEXTURE_CUBE_LAYER,
                                    .baseArrayLayer = 0,
                                    .mipLevelCount = 1,
                                    .baseMipLevel = glm_min(blur, mip_count),
                                });

  /* TODO OPTI:
     Currently use default box primitive which include
     normal/uv/color, but we actually only need position for the skybox, so
     maybe can use a "position-only" version to save a bit of memory
   */
  Primitive box_primitive = primitive_cube();
  Mesh *skybox_mesh = scene_new_mesh(scene);
  mesh_create_primitive(skybox_mesh, &(MeshCreatePrimitiveDescriptor){
                                         .name = "skybox mesh",
                                         .primitive = &box_primitive,
                                     });

  // assign shader
  mesh_shader_create_fixed(skybox_mesh, &(ShaderCreateDescriptor){
                                            .label = "skybox shader",
                                            .name = "skybox shader",
                                            .pipeline = std_render_pipeline(
                                                RenderPipelineType_Skybox),
                                        });

  // update texture and sampler
  Shader *shader = mesh_shader(skybox_mesh, MeshShader_Fixed);
  shader_update_texture_view(shader, 1, 0, *view,
                             TEXTURE_FORMAT_OFFSCREEN_DEFAULT);

  // add blur uniform
  shader_update_uniform_data(shader, 1, 2, (void *)&blur);

  // alter pipeline (no depth test)
  const RenderPipeline *pipeline = shader_pipeline(shader);

  scene_add_mesh_fixed(scene, skybox_mesh, ScenePipeline_Fixed_Background, NULL,
                       SceneAddFlag_Unselectable);
}

/**
   Create a skybox from a list of 6 textures.
 */
void prefab_skybox_create(Scene *scene,
                          const PrefabSkyboxCreateDescriptor *desc) {

  // create global texture
  SceneEnvironmentSkybox *scene_skybox =
      scene_environment_skybox(&scene->environment);

  WGPUTexture *skybox_texture = &scene_skybox->texture;
  WGPUTextureView *skybox_cubemap_view = &scene_skybox->view;

  *skybox_texture = prefab_skybox_texture(desc->resolution);

  Texture skybox_sides[TEXTURE_CUBE_LAYER];
  texture_create_cubemap_from_file(
      skybox_sides, &(TextureCreateCubeMapDescriptor){
                        .path = &desc->path,
                        .format = TEXTURE_FORMAT_OFFSCREEN_DEFAULT,
                        .resolution = desc->resolution,
                    });

  for (uint8_t i = 0; i < TEXTURE_CUBE_LAYER; i++)
    prefab_skybox_upload_layer(*skybox_texture, &skybox_sides[i], i,
                               BufferTextureMemory_Keep);

  prefab_skybox_create_from_texture(scene, *skybox_texture, skybox_cubemap_view,
                                    desc->resolution, desc->blur);
}

/**
   Create a gradient skybox from a list of gradient stops.
 */
void prefab_skybox_gradient_create(
    Scene *scene, const PrefabSkyboxGradientCreateDescriptor *desc) {

  SceneEnvironmentSkybox *scene_skybox =
      scene_environment_skybox(&scene->environment);

  WGPUTexture *skybox_texture = &scene_skybox->texture;
  WGPUTextureView *skybox_cubemap_view = &scene_skybox->view;

  // create global texture
  *skybox_texture = prefab_skybox_texture(desc->resolution);

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
  for (size_t i = 0; i < TEXTURE_CUBE_LAYER; i++) {

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
    prefab_skybox_upload_layer(*skybox_texture, final_texture, i, free_texture);
  }

  // free gradient texture
  stbi_image_free(gradient_texture.data);
  gradient_texture.data = NULL;

  prefab_skybox_create_from_texture(scene, *skybox_texture, skybox_cubemap_view,
                                    desc->resolution, 0.0f);
}
