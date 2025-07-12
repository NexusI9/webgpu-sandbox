#include "skybox.h"
#include "../../../backend/buffer.h"
#include "../../../include/stb/stb_image.h"
#include "../../primitive/cube.h"
#include "../../texture/texture.h"
#include "webgpu/webgpu.h"

void prefab_skybox_create(const PrefabCreateDescriptor *prefab,
                          const PrefabSkyboxCreateDescriptor *desc) {

  // create texture & global texture view
  const int channels = 3;
  const WGPUTextureFormat format = WGPUTextureFormat_RGBA8Unorm;
  const int layer_count = 6;

  const WGPUTexture skybox_texture = wgpuDeviceCreateTexture(
      *prefab->device,
      &(WGPUTextureDescriptor){
          .dimension = WGPUTextureDimension_2D,
          .format = format,
          .usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst,
          .sampleCount = 1,
          .mipLevelCount = 1,
          .size =
              (WGPUExtent3D){
                  .width = desc->resolution,
                  .height = desc->resolution,
                  .depthOrArrayLayers = layer_count,
              },
      });

  // put path in order
  const char *path_sort[6] = {
      desc->path.right,  // +X
      desc->path.left,   // -X
      desc->path.top,    // +Y
      desc->path.bottom, // -Y
      desc->path.front,  // +Z
      desc->path.back,   // -Z
  };

  // copy image to layer texture
  for (size_t i = 0; i < 6; i++) {
    const char *path = path_sort[i];

    Texture layer_texture;
    texture_create_from_file(&layer_texture, path, false);

    if (layer_texture.data == NULL) {
      perror("Couldn't read skybox texture.\n");
      return;
    } else {

      WGPUTextureView layer_texture_view = wgpuTextureCreateView(
          skybox_texture, &(WGPUTextureViewDescriptor){
                              .format = format,
                              .dimension = WGPUTextureViewDimension_2D,
                              .arrayLayerCount = 1,
                              .baseArrayLayer = i,
                              .mipLevelCount = 1,
                              .baseMipLevel = 0,
                          });

      buffer_create_texture_cube(&layer_texture_view,
                                 &(CreateTextureCubeDescriptor){
                                     .texture = &skybox_texture,
                                     .queue = prefab->queue,
                                     .width = layer_texture.width,
                                     .height = layer_texture.height,
                                     .size = layer_texture.size,
                                     .data = layer_texture.data,
                                     .channels = layer_texture.channels,
                                     .format = format,
                                     .layer = i,
                                 });
    }
  }

  // create global texture view
  WGPUTextureView skybox_texture_view = wgpuTextureCreateView(
      skybox_texture, &(WGPUTextureViewDescriptor){
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
