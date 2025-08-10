#include "billboard.h"
#include "../backend/renderer/scene/std_pipeline/std_pipeline.h"
#include "../runtime/primitive/plane.h"
#include "../runtime/texture/texture.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

/**
   Create a plane mesh with a billboard shader
 */
void seo_create_billboard(Mesh *mesh,
                          const SEOCreateBillboardDescriptor *desc) {

  // create plane
  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &plane,
                                  .device = desc->device,
                                  .queue = desc->queue,
                                  .name = "SEO Billboard",
                              });

  // assign billboard shader
  mesh_shader_create_fixed(mesh,
                           &(ShaderCreateDescriptor){
                               .device = desc->device,
                               .queue = desc->queue,
                               .label = "SEO billboard shader",
                               .name = "SEO billboard shader",
                               .pipeline = std_pipeline(PipelineType_Billboard),
                           });

  // set mesh position to light position
  mesh_translate(mesh, *desc->position);

  // scale down gizmo
  mesh_scale(mesh, *desc->scale);

  // TODO: create UI Atlas
  Texture light_texture;
  texture_create_from_file(&light_texture, desc->texture_path, true);

  // bind texture + sampler
  shader_update_texture(mesh_shader_fixed(mesh), 1, 0,
                        &(ShaderUpdateTexture){
                            .width = light_texture.width,
                            .height = light_texture.height,
                            .data = light_texture.data,
                            .size = light_texture.size,
                            .channels = light_texture.channels,
                            .dimension = WGPUTextureViewDimension_2D,
                            .format = WGPUTextureFormat_RGBA8Unorm,
                        });

  shader_update_sampler(mesh_shader_fixed(mesh), 1, 1,
                        &(WGPUSamplerDescriptor){
                            .addressModeU = WGPUAddressMode_Repeat,
                            .addressModeV = WGPUAddressMode_Repeat,
                            .addressModeW = WGPUAddressMode_Repeat,
                            .minFilter = WGPUFilterMode_Linear,
                            .magFilter = WGPUFilterMode_Linear,
                            .compare = WGPUCompareFunction_Undefined,
                        });

  const uint32_t size = 0;
  shader_update_uniform(mesh_shader_fixed(mesh), 1, 2, (void *)&size);
}
